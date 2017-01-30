//
//  AABoundingBox.cpp
//  ATTRACT
//
//  Created by Cody Rhoads on 1/29/17.
//
//

#include "AABoundingBox.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;
using namespace glm;

AABoundingBox::AABoundingBox() :
min(vec3(0.0f, 0.0f, 0.0f)),
max(vec3(0.0f, 0.0f, 0.0f))
{
    
}

AABoundingBox::AABoundingBox(const vec3 mins, const vec3 maxes) :
min(mins),
max(maxes)
{
    
}

AABoundingBox::~AABoundingBox()
{
    
}

bool AABoundingBox::isColliding(const shared_ptr<AABoundingBox> otherBox)
{
    if ((min.x <= otherBox->getMaxX() && max.x >= otherBox->getMinX()) &&
        (min.y <= otherBox->getMaxY() && max.y >= otherBox->getMinY()) &&
        (min.z <= otherBox->getMaxZ() && max.z >= otherBox->getMinZ())) {
        return true;
    }
    return false;
}
