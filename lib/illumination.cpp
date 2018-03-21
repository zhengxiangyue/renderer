//
// Created by Zheng XiangYue on 20/3/2018.
//

#include "illumination.h"

int diffuse_trem(double kd, double I_light, vector3d &normal, vector3d light) {

    double gradient = (normal.dot(light));

    return gradient > 0 ? gradient * kd * I_light : 0;
}