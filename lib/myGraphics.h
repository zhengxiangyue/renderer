//
// Created by Zheng XiangYue on 29/1/2018, G42416206
//

#ifndef GRAPHIC_MYGRAPHICS_H
#define GRAPHIC_MYGRAPHICS_H

#include <vector>
#include <iostream>
#include <cmath>
#include <fstream>
#include "matrix.h"

using namespace std;

/**
 * vector is denoted as (x, y, z)
 * the operation of a vector3d: + - *(corss product) dot()
 */
struct vector3d{

    double x,y,z;

    /**
     * constructor
     */
    vector3d(){};

    /**
     *
     * @param x
     * @param y
     * @param z
     */
    vector3d(double x, double y, double z):x(x),y(y),z(z){};

    /**
     *
     * @return
     */
    vector<double> get_vector();

    /**
     * vector cross product
     * @param vector3d end_vector
     * @return
     */
    vector3d operator *(vector3d end_vector);

    /**
     *
     * @param co
     * @return
     */
    vector3d operator *(double co);

    /**
     * vector minus
     * @param vector3d end_vector
     * @return vector3d
     */
    vector3d operator -(const vector3d end_vector);

    /**
     * vector addition
     * @param vector3d end_vector
     * @return vector3d
     */
    vector3d operator +(const vector3d end_vector);

    /**
     * vector divided by a scale
     * @param length
     * @return
     */
    vector3d operator /(const double length);

    /**
     * mold
     * @return
     */
    double mold();

    /**
     * dot product
     * @param end_vector
     * @return
     */
    double dot(vector3d end_vector);
};

/**
 * point is denoted by x, y, z coordinates
 */
struct point3d{
    double x,y,z;
    point3d(){};
    point3d(double x, double y, double z):x(x),y(y),z(z){};
    vector<double> get_vector();

    vector3d operator -(point3d end_point);
    matrix<double> to_matrix();
};

/**
 * object_point is a point with objec propoty
 */
struct object_point3d : public point3d {

};


/**
 * edge table element
 */
struct edge_table_element{
    int y_start;          // same as the element index in the edge table
    int y_max;          // upper point y

    double x_start;       // corespond to y_start
    double delta;       // delta x / delta y

    double z_start;         // for z_buffer y increment
    double z_delta_to_y;      //

    pair<double, double> texture_start, texture_delta_to_y;

    vector3d normal_start;
    vector3d normal_end;

    edge_table_element(int y_start, int y_max, double x_start, double delta, double z_start, double z_delta_to_y, vector3d normal_start, vector3d normal_end, pair<double,double> texture_start, pair<double, double> texture_delta_to_y) :
            y_start(y_start),
            y_max(y_max),
            x_start(x_start),
            delta(delta),
            z_start(z_start),
            z_delta_to_y(z_delta_to_y),
            normal_start(normal_start),
            normal_end(normal_end),
            texture_start(texture_start),
            texture_delta_to_y(texture_delta_to_y)
    {}
};

struct act_cmp{
    bool operator ()(edge_table_element& a, edge_table_element& b) {
        return a.x_start < b.x_start;
    }
};

/**
 * stream operator << overlodaded for vector3d
 * @param co
 * @param vec
 * @return
 */
ostream& operator <<(ostream &co, const point3d &vec);

/**
 * stream operator << overlodaded for vector3d
 * @param co
 * @param vec
 * @return
 */
ostream& operator <<(ostream &co, const vector3d &vec);


/**
 * several points consist the object
 * while faces are denoted by the point index, clockwise
 */
struct  polygonal_object{

    /**
     * points in 3d world
     */
    vector<point3d> points;

    /**
     * [[0,1,2],[2,3,4]] means there are two polygons in the object
     * the first face is made by 3 points: index 0, index 1, and index2
     */
    vector<vector<int>> faces;

    /**
     * New added, when all
     */
    vector<vector3d> point_normal;

    /**
     * Each pholygon has a normal, accessed through
     */
    vector<vector3d> face_normal;

    /**
     * the face_index-th face's normal vector, for the purpose of calculating  face_normal
     * @param int face_index
     * @return vector3d
     */
    vector3d normal(vector<int> &face, int& face_index);

    /**
     * Calculate each point's normal
     */
    void compute_secene_point_normal();
};

/**
 *
 */
struct _light{
    point3d position;
    int intensity;
    _light(point3d position, int intensity):position(position), intensity(intensity){};
};

vector<vector<vector<uint8_t>>> read_bmp_to_buffer(const char* filename);

double co_to_degree(double cosa_t, double sina_t);

#endif //GRAPHIC_MYGRAPHICS_H