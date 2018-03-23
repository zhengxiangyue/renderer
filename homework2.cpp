//
// Created by Zheng XiangYue on 9/2/2018.
//

#include "homework2.h"

/* homework2 */

homework2::homework2() {
    window_x = WINDOW_X;
    window_y = WINDOW_Y;

    /* Initialize refresh buffer, 3 channels*/

    /* Initialize z buffer, the value is between 0 ~ 1, double */
    z_buffer = vector<vector<double>>(window_x, vector<double>(window_y,1));

    edge_table = vector<vector<edge_table_element>>(window_y);

    for(int i = 0 ; i < WINDOW_X ; ++i) {
        for(int j = 0 ; j < WINDOW_Y ; ++j) {
            pixel_buffer[i][j][0] = 0;
            pixel_buffer[i][j][1] = 0;
            pixel_buffer[i][j][2] = 0;
        }
    }
}

/**
 * For each polygon do scan conversion
 * Step 1: Add all newly intersect edges
 * Step 2: Remove all leaving edges
 * Step 3: Update all intersection information
 */
void homework2::scan_conversion(bool single_light_on) {
    /* each loop handel one face*/
    srand(time(NULL));

    set<int> red_set;

    /* May render several times, initialize the buffers */

    z_buffer = vector<vector<double>>(window_x, vector<double>(window_y,1));

    for(int i = 0 ; i < WINDOW_X ; ++i) {
        for(int j = 0 ; j < WINDOW_Y ; ++j) {
            pixel_buffer[i][j][0] = 200;
            pixel_buffer[i][j][1] = 200;
            pixel_buffer[i][j][2] = 200;
        }
    }

    /* Scan conversion for each face */
    for(int i = 0 ; i < object.faces.size() ; ++i) {

        // only give color when first time render
        if(face_color_r_buffer.size() == i){

            if(single_light_on) {

                uint8_t gray = 0;

                vector3d normal = object.face_normal[i];

                for(auto each:lights) {
                    vector3d light_vector = each - point3d(0,0,0);
                    gray += (128 + 128 * (normal / normal.mold()).dot(light_vector / light_vector.mold()));
                }

                face_color_r_buffer.push_back(gray);
                face_color_g_buffer.push_back(gray);
                face_color_b_buffer.push_back(gray);
            }else {
                uint8_t color_r = rand() % 255, color_g = rand() % 255, color_b = rand() % 255;
                face_color_r_buffer.push_back(color_r);
                face_color_g_buffer.push_back(color_g);
                face_color_b_buffer.push_back(color_b);
            }
        }

        // Do not consider back face, make sure
//        if(back_face_indexs.find(i) != back_face_indexs.end())
//            continue;

        auto each_face = object.faces[i];
        int point_number = each_face.size();

        /* build this face's edge table, before that, clear the edge table */

        for(auto& each:edge_table)
            if(each.size()) each.clear();

        for(int j = 0 ; j < point_number ; ++j) {
            // add each edge to the edge table
            // two point indexs are each_face[j], each_face[(j+1)%each_face.size()]
            int index_start = each_face[j], index_end = each_face[(j+1)%point_number];
            auto lower_point = screen_points[index_start], upper_point = screen_points[index_end];

            // if out of bound, exit, TODO: not exit...
            if(lower_point.y > 1 || lower_point.y < -1 || lower_point.x > 1 || lower_point.x < -1 ||
                    upper_point.y > 1 || upper_point.y < -1 || upper_point.x > 1 || upper_point.x < -1)
                exit(12);

            // if it is a horizontal edge, give color directly
//            if(to_pixel(lower_point.y) == to_pixel(upper_point.y))
//                continue;

            if(lower_point.y > upper_point.y) {
                swap(lower_point, upper_point);
                swap(index_start, index_end);
            }


            // Calculate the intensity of the lower poi

            edge_table_element new_element(to_pixel(lower_point.y),
                                           to_pixel(upper_point.y),
                                           to_double_pixel(lower_point.x),
                                           (double)(lower_point.x - upper_point.x) / (double)(lower_point.y - upper_point.y),
                                           lower_point.z,
                                           (double)(lower_point.z - upper_point.z) / (double)(to_pixel(lower_point.y) - to_pixel(upper_point.y)),
                                           object.point_normal[index_start],
                                           object.point_normal[index_end]
            );

            edge_table[to_pixel(lower_point.y)].push_back(new_element);
        }

        /* the edge table has been built, use scan line to decide the pixels' color */

        // In the edge table, x, y are from 0 to WINDOW_SIZE

        // ei is the current scan line's y coordinate
        int ei = 0;

        // find the first y place to start todo: edge_table as data structure
        for(; ei < window_y ; ++ei) if(edge_table[ei].size()) break;

        // each loop scan each line
        for(; ei < window_y ; ++ei) {
            // move edge(s) from ET bucket y whose ymin = y (entering edges) to ATE,
            for(int j = 0 ; j < edge_table[ei].size() ; ++j)
                active_edge_table.push_back(edge_table[ei][j]);

            edge_table[ei].clear();

            // Remove the leaving edge
            vector<edge_table_element>::iterator it = active_edge_table.begin();
            while(it != active_edge_table.end()) {
                if(ei == it->y_max)
                    // Remove from AET entries for which y = ymax (leave edges)
                    active_edge_table.erase(it);
                else
                    // replace x by x + increment
                    // This places next scan-line intersection into each entry in AET
                    it++;
            }

            sort(active_edge_table.begin(), active_edge_table.end(), act_cmp());

            //  Fill in desired pixel values on scan line y by using pairs of xcoordinates from the AET
            for(double k = 0 ; k < active_edge_table.size() ; k += 2) {
                edge_table_element *span_left = &active_edge_table[k], *span_right = &active_edge_table[k+1];
                double z_current = span_left->z_start, z_delta_to_x = (span_left->z_start - span_right->z_start) / (span_left->x_start - span_right->x_start);

                // According to intensity
                vector3d la = span_left->normal_end * (double)(ei - span_left->y_start)/(double)(span_left->y_max - span_left->y_start)
                              + span_left->normal_start * (double)(span_left->y_max - ei)/(double)(span_left->y_max - span_left->y_start),

                lb = span_right->normal_end * (double)(ei - span_right->y_start)/(double)(span_right->y_max - span_right->y_start)
                     + span_right->normal_start * (double)(span_right->y_max - ei)/(double)(span_right->y_max - span_right->y_start);

                for(int l = (int)span_left->x_start  ; l <= (int)span_right->x_start ; ++l) {
                    bool covered = false;
                    if(z_current <= z_buffer[l][ei]){

                        if(z_buffer[l][ei] != 1) covered = true;

                        z_buffer[l][ei] = z_current;

                        vector3d cur_normal = span_right->x_start - span_left->x_start == 0 ? la : (la * (span_right->x_start - (double)l) / (span_right->x_start - span_left->x_start) +
                                lb * ((double)l - span_left->x_start) / (span_right->x_start - span_left->x_start));

                        // Normalizing is so ******* important
                        cur_normal = cur_normal / cur_normal.mold();

                        int gray = 0;
                        for(auto each:lights) {
                            vector3d light_vector = point3d(0,0,0) - each;

                            light_vector = light_vector / light_vector.mold();

                            gray += diffuse_trem(1,88,cur_normal,light_vector);
                        }
//                        if(gray == 0) {
//                            cout << "(" << ei << "," << l << ")" << endl;
//                            gray = 255;
//                        }
                        // gray;//

                        if(back_face_indexs.find(i) != back_face_indexs.end()) {
                            pixel_buffer[ei][l][0] = covered ? 255 : 0;
                            pixel_buffer[ei][l][1] = covered ? 0 : 255;
                            pixel_buffer[ei][l][2] = 0;
                            red_set.insert(i);
                        }else {
//                            cout << gray << endl;
                            pixel_buffer[ei][l][0] = gray;//face_color_r_buffer[i];
                            pixel_buffer[ei][l][1] = gray;//face_color_r_buffer[i];
                            pixel_buffer[ei][l][2] = gray;//face_color_r_buffer[i];
                        }

//                        pixel_buffer[ei][l][0] = 255;
//                        pixel_buffer[ei][l][1] = 255;
//                        pixel_buffer[ei][l][2] = 255;

                    }

                    z_current += z_delta_to_x;
                }

                // Update x and z
                span_left->x_start += span_left->delta;
                span_right->x_start += span_right->delta;

                span_left->z_start += span_left->z_delta_to_y;
                span_right->z_start += span_right->z_delta_to_y;
            }
        }
    }

    cout << "Size:" << back_face_indexs.size() << "," << red_set.size() << endl;
}

inline int homework2::to_pixel(double &value, bool shortten) {
    // [-1, 1] is transformed to [0, WINDOW_X]
    return floor((value+1)*window_y/2) - shortten;
}

inline double homework2::to_double_pixel(double &value) {
    return (value+1)*window_y/2.0;
}

void homework2::set_single_light_position(const point3d input_light) {
    lights.push_back(input_light);
}



