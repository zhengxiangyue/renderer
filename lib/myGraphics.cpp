//
// Created by Zheng XiangYue on 29/1/2018, G42416206
//

#include "myGraphics.h"
#include "matrix.h"

/**
 *
 * @return
 */
vector<double> point3d::get_vector() {
    return {x,y,z};
}

/**
 *
 * @param end_point
 * @return
 */
vector3d point3d::operator-(point3d end_point)  {
    return vector3d(x - end_point.x, y - end_point.y, z - end_point.z);
}

matrix<double> point3d::to_matrix() {
    return matrix<double>({{x},{y},{z}});
}

/**
 *
 * @return
 */
vector<double> vector3d::get_vector() {
    return {x,y,z};
}

/**
 *
 * @param end_vector
 * @return
 */
vector3d vector3d::operator*(vector3d end_vector) {
    vector3d new_vector;
    new_vector.x = y * end_vector.z - end_vector.y * z;
    new_vector.y = -(x * end_vector.z - end_vector.x * z);
    new_vector.z = x * end_vector.y - end_vector.x * y;
    return new_vector;
}

/**
 *
 * @param end_vector
 * @return
 */
double vector3d::dot(vector3d end_vector) {
    return x * end_vector.x + y * end_vector.y + z * end_vector.z;
}

/**
 *
 * @param end_vector
 * @return
 */
vector3d vector3d::operator-(const vector3d end_vector) {
    vector3d new_vector;
    new_vector.x = x - end_vector.x;
    new_vector.y = x - end_vector.y;
    new_vector.z = x - end_vector.z;
    return new_vector;
}

/**
 *
 * @param end_vector
 * @return
 */
vector3d vector3d::operator+(const vector3d end_vector) {
    vector3d new_vector;
    new_vector.x = x + end_vector.x;
    new_vector.y = x + end_vector.y;
    new_vector.z = x + end_vector.z;
    return new_vector;
}

/**
 *
 * @param length
 * @return
 */
vector3d vector3d::operator/(const double length) {
    vector3d new_vector;
    new_vector.x = x / length;
    new_vector.y = y / length;
    new_vector.z = z / length;
    return new_vector;
}

/**
 *
 * @return
 */
double vector3d::mold() {
    return sqrt(x*x + y*y + z*z);
}

/**
 * the vector will be shown as (x,y,z)
 * @param co
 * @param vec
 * @return
 */
ostream &operator <<(ostream &co, const vector3d &vec) {
    co << "(" << vec.x << " " << vec.y << " " << vec.z << ")";
    return co;
}

ostream &operator <<(ostream &co, const point3d &point) {
    co << "(" << point.x << " " << point.y << " " << point.z << ")";
    return co;
}


vector3d polygonal_object::normal(vector<int> &face) {

    point3d first_point = points[face[0]], second_point = points[face[1]], third_point = points[face[2]];

    vector3d first_vector = second_point - first_point, second_vector = third_point - second_point;

    return first_vector * second_vector;
}
