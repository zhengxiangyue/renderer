//
// Created by Zheng XiangYue on 20/3/2018.
//

#ifndef GRAPHIC_ILLUMINATION_H
#define GRAPHIC_ILLUMINATION_H

#include "myGraphics.h"
#include <cmath>

int diffuse_term(double kd, double I_light, vector3d &normal, vector3d light, int power = 1);

int specular_term(double k, double I, vector3d &normal,  vector3d &light,  vector3d &view, const int n);

#endif //GRAPHIC_ILLUMINATION_H
