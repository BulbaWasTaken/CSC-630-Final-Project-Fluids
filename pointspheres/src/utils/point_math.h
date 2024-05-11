#ifndef POINT_MATH_H
#define POINT_MATH_H

#include <cmath>

class PointMath {
public:
    static const float E_GRAVITY;
    static const float KERNEL_RADIUS;
    static float poly6(float radius, float distance);
};

#endif