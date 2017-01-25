//
//  BoundingSphere.cpp
//  Lab1
//
//  Created by Cody Rhoads on 1/16/17.
//
//

#include <iostream>

#include "BoundingSphere.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;
using namespace glm;

BoundingSphere::BoundingSphere() :
center(vec3(0.0f, 0.0f, 0.0f)),
radius(0.0f)
{
    
}

BoundingSphere::BoundingSphere(const vec3 &center, float radius) :
center(center),
radius(radius)
{
    
}

BoundingSphere::~BoundingSphere()
{
    
}

bool BoundingSphere::isColliding(const std::shared_ptr<BoundingSphere> &otherSphere)
{
    return findDistance(center, otherSphere->getCenter()) <= radius + otherSphere->getRadius();
}

float BoundingSphere::findDistance(const glm::vec3 &p1, const glm::vec3 &p2)
{
    float diffX = p2[0] - p1[0];
    float diffY = p2[1] - p1[1];
    float diffZ = p2[2] - p1[2];
    
    return sqrt((diffX * diffX) + (diffY * diffY) + (diffZ * diffZ));
}
