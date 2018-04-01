//
// Created by Zheng XiangYue on 28/1/2018. G 42416206
//

#ifndef GRAPHICS_HOMEWORK1_H
#define GRAPHICS_HOMEWORK1_H

#include "lib/myGraphics.h"
#include "lib/matrix.h"
#include "lib/illumination.h"
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <unordered_set>
#include <set>

#define WINDOW_X                    1000
#define WINDOW_Y                    1000

#define DEBUG_MODE                  false

#define CLOCK_WISE_FACE             false

#define SHADING_CONSTANT            1
#define SHADING_GOURAUD             2
#define SHADING_PHONG               3

#define ILLUMINATION_DIFFUSE        1
#define ILLUMINATION_MULTIPLE       2

#define oops(m,x){perror(m); exit(x);}


using namespace std;

class homework1 {
public:
    /**
     * C -> camera position denoted by a 3d point
     */
    point3d camera_position;

    /**
     * Pref -> camera is looking at this point
     */
    point3d viewing_point;

    /**
     * V' -> the up vector of the camera
     */
    vector3d camera_up_direction;

    /**
     * The object， actually, all polygons of all object are stored here
     */
    polygonal_object object;

    /**
     * Use 3 vectors to denote the camera coordinates
     */
    vector3d N, U, V;

    /**
     * screen pixel points transformed from the object using perspective transform, with z depth
     * x,y maps to -1 ~ 1, z maps to 0 ~ 1
     * */
    vector<point3d> screen_points;

    /**
     * Back face indexs are stored and the faces are not shown when drawing
     */
    unordered_set<int> back_face_indexs;

    /**
     * Read from a ".d" file which contain the information of the object
     */
    bool set_object_position(const char*, const vector<double>);

    /**
     * Read from a ".txt" file which contain 9 numbers to denote the pattern of the camera
     */
    void set_camera_position(const char*);

    /**
     * Calculate the back faces base on the current scene
     */
    void denote_back_face();

    /**
     * Perspective transform, after the transform, x is [-1,1], y is [-1,1], z is [0,1]
     */
    void object_points_to_screen_points();
};



#endif //GRAPHICS_HOMEWORK1_H