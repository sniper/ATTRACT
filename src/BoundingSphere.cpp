//
//  BoundingSphere.cpp
//  Lab1
//
//  Created by Cody Rhoads on 1/16/17.
//
//

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "BoundingSphere.hpp"
#include "AABoundingBox.hpp"
#include "Calculations.hpp"

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

bool BoundingSphere::isCollidingWithSphere(const std::shared_ptr<BoundingSphere> sphere)
{
    return Calculations::findDistance(center, sphere->getCenter()) <= radius + sphere->getRadius();
}

bool BoundingSphere::isCollidingWithAABox(const std::shared_ptr<AABoundingBox> box)
{
    // Needs implementation at some point.
    return false;
}
