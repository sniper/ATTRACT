//
//  Calculations.cpp
//  ATTRACT
//
//  Created by Cody Rhoads on 2/7/17.
//
//

#include "Calculations.hpp"

using namespace glm;

float Calculations::findDistance(const vec3 &p1, const vec3 &p2)
{
    float diffX = p2.x - p1.x;
    float diffY = p2.y - p1.y;
    float diffZ = p2.z - p1.z;
    
    return sqrt((diffX * diffX) + (diffY * diffY) + (diffZ * diffZ));
}
