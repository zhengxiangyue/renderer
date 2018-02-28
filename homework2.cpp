//
// Created by Zheng XiangYue on 9/2/2018.
//

#include "homework2.h"

/* homework2 */

#define WINDOW_X 768
#define WINDOW_Y 768

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


void homework2::scan_conversion(bool single_light_on) {
    /* each loop handel one face*/
    srand(time(NULL));


    /* May render several times, initialize the buffers */

    z_buffer = vector<vector<double>>(window_x, vector<double>(window_y,1));

    for(int i = 0 ; i < WINDOW_X ; ++i) {
        for(int j = 0 ; j < WINDOW_Y ; ++j) {
            pixel_buffer[i][j][0] = 0;
            pixel_buffer[i][j][1] = 0;
            pixel_buffer[i][j][2] = 0;
        }
    }

    /* Scan conversion for each face */
    for(int i = 0 ; i < object.faces.size() ; ++i) {

        // only give color when first time render
        if(face_color_r_buffer.size() == i){

            if(single_light_on) {

                uint8_t gray = 0;

                vector3d normal = object.normal(object.faces[i]);

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
        if(back_face_indexs.find(i) != back_face_indexs.end())
            continue;

        auto each_face = object.faces[i];
        int point_number = each_face.size();


        /* build this face's edge table, before that, clear the edge table */

        for(auto& each:edge_table)
            if(each.size()) each.clear();

        for(int j = 0 ; j < point_number ; ++j) {
            // add each edge to the edge table
            // two point indexs are each_face[j], each_face[(j+1)%each_face.size()]
            auto lower_point = screen_points[each_face[j]], upper_point = screen_points[each_face[(j+1)%point_number]];

            // if it is a horizontal edge, ignore it
            if(to_pixel(lower_point.y) == to_pixel(upper_point.y))
                continue;

            if(lower_point.y >  upper_point.y)
                swap(lower_point, upper_point);

            // if the lower point out of screen, ignore it
            if(lower_point.y > 1 || lower_point.y < -1)
                continue;

            edge_table_element new_element(to_pixel(lower_point.y),
                               to_pixel(upper_point.y, true),
                               to_double_pixel(lower_point.x),
                               (double)(lower_point.x - upper_point.x) / (double)(lower_point.y - upper_point.y),
                                           upper_point.z,
                                           lower_point.z
            );

            if(new_element.y_start > new_element.y_max)
                new_element.y_max = new_element.y_start;

            int edge_table_size = edge_table.size();
            edge_table[to_pixel(lower_point.y)].push_back(new_element);
        }

        /* the edge table has been built, use scan line to decide the pixels' color */

        // ei is the current scan line's y coordinate
        int ei = 0;

        // find the first y place to start
        for(; ei < window_y ; ++ei) if(edge_table[ei].size()) break;

        // each loop scan each line
        for(; ei < window_y ; ++ei) {
            // move edge(s) from ET bucket y whose ymin = y (entering edges)
            // to ATE,
            for(int j = 0 ; j < edge_table[ei].size() ; ++j)
                active_edge_table.push_back(edge_table[ei][j]);

            edge_table[ei].clear();

            // maintaining ATE sort order on x
            sort(active_edge_table.begin(), active_edge_table.end(), act_cmp());

            //  Fill in desired pixel values on scan line y by using pairs of xcoordinates from the AET
            for(double k = 0 ; k < active_edge_table.size() ; k += 2) {
                edge_table_element span_left = active_edge_table[k], span_right = active_edge_table[k+1];

                if((int)span_left.x_start > (int)span_right.x_start)
                    continue;

                double za = span_left.y_max == span_left.y_start ?  span_left.z_upper : span_left.z_upper - (span_left.z_upper - span_left.z_lower) * (span_left.y_max - ei) / (span_left.y_max - span_left.y_start);
                double zb = span_right.y_max == span_right.y_start ? span_right.z_upper : span_right.z_upper - (span_right.z_upper - span_right.z_lower) * (span_right.y_max - ei) / (span_right.y_max - span_right.y_start);

                // todo: z-buffer
                for(int l = max(0,(int)span_left.x_start)  ; l <= (int)span_right.x_start && l < WINDOW_X ; ++l) {

                    // calculate z value of the current point
                    double zp = l == max(0,(int)span_left.x_start) ? za : zb - (zb - za) * (span_right.x_start - l)  / (span_right.x_start - span_left.x_start);

                    if(zp > z_buffer[l][ei])
                        continue;

                    z_buffer[l][ei] = zp;

                    // exchange ei and l so that the coordinates match

                    pixel_buffer[ei][l][0] = face_color_r_buffer[i];
                    pixel_buffer[ei][l][1] = face_color_g_buffer[i];
                    pixel_buffer[ei][l][2] = face_color_b_buffer[i];


                }
            }


            vector<edge_table_element>::iterator it = active_edge_table.begin();

            while(it != active_edge_table.end()) {
                if(ei == it->y_max) {
                    // Remove from AET entries for which y = ymax (leave edges)
                    active_edge_table.erase(it);
                }else {
                    // replace x by x + increment
                    // This places next scan-line intersection into each entry in AET
                    it->x_start += it->delta;
                    it++;
                }
            }

            sort(active_edge_table.begin(), active_edge_table.end(), act_cmp());

        }
    }
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



