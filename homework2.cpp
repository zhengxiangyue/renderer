//
// Created by Zheng XiangYue on 9/2/2018.
//

#include "homework2.h"

/* homework2 */

static unordered_set<int> suspect = {
        2710
};


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
    z_buffer = vector<vector<double>>(window_x, vector<double>(window_y,1));

    /* May render several times, initialize the buffers with background color */
    for(int i = 0 ; i < WINDOW_X ; ++i) {
        for(int j = 0 ; j < WINDOW_Y ; ++j) {
            pixel_buffer[i][j][0] = _back_r;
            pixel_buffer[i][j][1] = _back_g;
            pixel_buffer[i][j][2] = _back_b;
        }
    }

    /* Scan conversion for each face */
    for(int i = 0 ; i < object.faces.size() ; ++i) {

        // Do not consider back face
        if(back_face_indexs.find(i) != back_face_indexs.end())
            continue;

        auto each_face = object.faces[i];
        int point_number = each_face.size();



        int _gray = 0;

        for(auto each:lights) {
            vector3d light_vector = each.position - point3d(0,0,0);
            light_vector = light_vector / light_vector.mold();
            _gray += diffuse_term(1,each.intensity,object.face_normal[i],light_vector,15);
        }

        _gray += 60;

        uint8_t constant_sading_color = _gray;

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

            // if it is a horizontal edge, don't worry
            if(to_pixel(lower_point.y) == to_pixel(upper_point.y))
                continue;

            if(lower_point.y > upper_point.y) {
                swap(lower_point, upper_point);
                swap(index_start, index_end);
            }

            edge_table_element new_element(to_pixel(lower_point.y, true),
                                           to_pixel(upper_point.y, true),
                                           to_double_pixel(lower_point.x),
                                           (double)(lower_point.x - upper_point.x) / (double)(lower_point.y - upper_point.y),
                                           lower_point.z,
                                           (double)(lower_point.z - upper_point.z) / (double)(to_pixel(lower_point.y, true) - to_pixel(upper_point.y, true)),
                                           object.point_normal[index_start],
                                           object.point_normal[index_end]
            );

            edge_table[to_pixel(lower_point.y, true)].push_back(new_element);
        }

        /* the edge table has been built, use scan line to decide the pixels' color */

        // In the edge table, x, y are from 0 to WINDOW_SIZE

        // ei is the current scan line's y coordinate
        int ei = 0;

        // find the first y place to start TODO: edge_table as data structure
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

                // any time span_left->y_max >= span_left->y_start
                vector3d
                        la = span_left->normal_end * (double)(ei - span_left->y_start)/(double)(span_left->y_max - span_left->y_start)
                         + span_left->normal_start * (double)(span_left->y_max - ei)/(double)(span_left->y_max - span_left->y_start),

                        lb = span_right->normal_end * (double)(ei - span_right->y_start)/(double)(span_right->y_max - span_right->y_start)
                         + span_right->normal_start * (double)(span_right->y_max - ei)/(double)(span_right->y_max - span_right->y_start);

//                if((int)span_left->x_start == (int)span_right->x_start)
//                    cout << "Single point" << endl;

                // pixel scaned horizontally
                for(int l = int(span_left->x_start)  ; l <= int(span_right->x_start) ; ++l) {

//                    if(l == (int)span_right->x_start && SHADING_CONSTANT == _shading_model && (int)span_left->x_start == (int)span_right->x_start )
//                        cout << "Problem" << endl;

                    if(z_current < z_buffer[l][ei]){

                        // update depth buffer
                        z_buffer[l][ei] = z_current;

                        // Phong illumination is based on normal
                        vector3d cur_normal;

                        if(l == (int)span_left->x_start) {
                            if(ei == span_left->y_start)
                                cur_normal = span_left->normal_start;
                            else
                                cur_normal = la;
                        }else {
                            cur_normal = la * (span_right->x_start - l) + lb * (l - span_left->x_start);
                        }

                        // Normalizing is so important
                        cur_normal = cur_normal / cur_normal.mold();

                        // for each light, give light on the object

                        int gray = 0;

                        for(auto each:lights) {
                            vector3d light_vector = each.position - point3d(0,0,0);
                            light_vector = light_vector / light_vector.mold();
                            int n = i >= 1890 ? 9 : 1;
                            gray += diffuse_term(2,each.intensity,cur_normal,light_vector,n);
                        }

                        gray += (i >= 1890 ? 30 : 80) ;

                        if(gray < 0) gray = 0;
                        if(gray > 255) gray = 255;

                        if(i <= 1)
                            gray = 230;
                        else if(i <= 3)
                            gray = 20;


                        if(_shading_model == SHADING_CONSTANT) {
//                            if(l == (int)span_left->x_start) {
//                                pixel_buffer[ei][l][0] = 255;
//                                pixel_buffer[ei][l][1] = 255;
//                                pixel_buffer[ei][l][2] = 255;
//                            }else {
                                pixel_buffer[ei][l][0] = constant_sading_color;
                                pixel_buffer[ei][l][1] = constant_sading_color;
                                pixel_buffer[ei][l][2] = constant_sading_color;
//                            }


                        }else {
//                            if(l == (int)span_left->x_start) {
//                                pixel_buffer[ei][l][0] = constant_sading_color;
//                                pixel_buffer[ei][l][1] = constant_sading_color;
//                                pixel_buffer[ei][l][2] = constant_sading_color;
//                            }else {
                                pixel_buffer[ei][l][0] = gray;
                                pixel_buffer[ei][l][1] = gray;
                                pixel_buffer[ei][l][2] = gray;
//                            }
                        }
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
//    cout << "______" << endl;

}

inline int homework2::to_pixel(double &value, bool shortten) {
    // [-1, 1] is transformed to [0, WINDOW_X]
    return floor((value+1)*window_y/2) - shortten;
}

inline double homework2::to_double_pixel(double &value) {
    return (value+1)*window_y/2.0;
}

void homework2::set_single_light_position(const point3d input_light, int intensity) {
    lights.push_back(_light(input_light,intensity));
}

void homework2::set_shading_model(int shading_model) {
    _shading_model = shading_model;
}

void homework2::set_background_color(uint8_t r, uint8_t g, uint8_t b) {
    _back_r = r, _back_g = g, _back_b = b;
}

void homework2::set_illumination_model(int model) {

}

/**
 *
 * @param face_index
 */
int homework2::_give_constant_shading_color(const int &face_index, int ambiant_term = 30) {

    int gray = 0;

    for(auto each:lights) {
        vector3d light_vector = each.position - point3d(0,0,0);
        light_vector = light_vector / light_vector.mold();
        gray += diffuse_term(1, each.intensity, object.face_normal[face_index], light_vector);
    }

    gray += ambiant_term;

    if( gray > 255 )
        return 255;
    if( gray < 0)
        return 0;

    return gray;

}



