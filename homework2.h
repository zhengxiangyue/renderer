//
// Created by Zheng XiangYue on 9/2/2018. G42416206
//

#ifndef GRAPHIC_HOMEWORK2_H
#define GRAPHIC_HOMEWORK2_H
#include "homework1.h"
#include <queue>

struct act_cmp{
    bool operator ()(edge_table_element& a, edge_table_element& b) {
        return a.x_start < b.x_start;
    }
};

class homework2:public homework1{
public:

    /**
     * screen pixel number vertically
     */
    int window_x;

    /**
     * screen pixel number horizontally
     */
    int window_y;

    /**
     * refresh buffer R G B
     */
    uint8_t pixel_buffer[WINDOW_X][WINDOW_Y][3];

    /**
     * Single light source
     */
    vector<point3d> lights;

    /**
     * Set single light source position
     */
    void set_single_light_position(point3d);

    /**
     * z buffer
     */
    vector<vector<double>> z_buffer;

    vector<uint8_t> face_color_r_buffer;
    vector<uint8_t> face_color_g_buffer;
    vector<uint8_t> face_color_b_buffer;

    /**
     * the object has an edge table
     */
    vector<vector<edge_table_element>> edge_table;

    /**
     * use active edge table to update the buffers
     */
    vector<edge_table_element> active_edge_table;

    /**
     * constructor
     */
    homework2();

    int to_pixel(double &value, bool shortten = false);

    double to_double_pixel(double &value);

    /**
     * for each polygon, create its edge_table
     */
    void scan_conversion(bool single_light_on = false);
};


#endif //GRAPHIC_HOMEWORK2_H
