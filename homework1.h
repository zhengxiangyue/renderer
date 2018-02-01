//
// Created by Zheng XiangYue on 28/1/2018. G 42416206
//

#ifndef GRAPHICS_HOMEWORK1_H
#define GRAPHICS_HOMEWORK1_H

#include "lib/myGraphics.h"
#include "lib/matrix.h"
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <unordered_set>

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
     * The object
     */
    polygonal_object object;

    /**
     * Use 3 vectors to denote the camera coordinates
     */
    vector3d N, U, V;

    /**
     * Back face indexs are stored and the faces are not shown when drawing
     */
    unordered_set<int> back_face_indexs;

    /**
     * Read from a ".d" file which contain the information of the object
     */
    bool set_object_position(const char*);

    /**
     * Read from a ".txt" file which contain 9 numbers to denote the pattern of the camera
     */
    void set_camera_position(const char*);

    /**
     * Calculate the back faces base on the current scene
     */
    void denote_back_face();
};



#endif //GRAPHICS_HOMEWORK1_H