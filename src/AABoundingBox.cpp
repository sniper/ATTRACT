//
//  AABoundingBox.cpp
//  ATTRACT
//
//  Created by Cody Rhoads on 1/29/17.
//
//

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "AABoundingBox.hpp"
#include "BoundingSphere.hpp"

using namespace std;
using namespace glm;

AABoundingBox::AABoundingBox() :
position(vec3(0.0f, 0.0f, 0.0f)),
halfExtents(vec3(0.0f, 0.0f, 0.0f))
{
    
}

AABoundingBox::AABoundingBox(const vec3 position, const vec3 halfExtents) :
position(position),
halfExtents(halfExtents)
{
    
}

AABoundingBox::~AABoundingBox()
{
    
}

bool AABoundingBox::isCollidingWithSphere(const std::shared_ptr<BoundingSphere> sphere)
{
    // Needs implementation at some point.
    return false;
}

bool AABoundingBox::isCollidingWithAABox(const std::shared_ptr<AABoundingBox> box)
{
    if ((getMins().x <= box->getMaxes().x && getMaxes().x >= box->getMins().x) &&
        (getMins().y <= box->getMaxes().y && getMaxes().y >= box->getMins().y) &&
        (getMins().z <= box->getMaxes().z && getMaxes().z >= box->getMins().z)) {
        return true;
    }
    return false;
}
