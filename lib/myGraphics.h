//
// Created by Zheng XiangYue on 29/1/2018, G42416206
//

#ifndef GRAPHIC_MYGRAPHICS_H
#define GRAPHIC_MYGRAPHICS_H

#include <vector>
#include <iostream>
#include <cmath>
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
 * edge table element
 */
struct edge_table_element{
    int y_start;          // same as the element index in the edge table
    int y_max;          // upper point y
    double x_start;       // corespond to x_min
    double delta;       // delta x / delta y

    double z_upper;
    double z_lower;

    edge_table_element(int y_start, int y_max, double x_start, double delta, double z_upper, double z_lower) : y_start(y_start),
                                                                           y_max(y_max),
                                                                           x_start(x_start),
                                                                           delta(delta), z_upper(z_upper),z_lower(z_lower){}
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
struct polygonal_object{

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
     * the face_index-th face's normal vector
     * @param int face_index
     * @return vector3d
     */
    vector3d normal(vector<int> &face);
};


#endif //GRAPHIC_MYGRAPHICS_H