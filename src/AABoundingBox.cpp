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

/*make sure to delete this bad boy after your done with it*/
std::vector<glm::vec3>* AABoundingBox::getVertices() {
    vector<glm::vec3> *ret = new vector<glm::vec3>();
    glm::vec3 p1 = getMins();
    glm::vec3 p2 = getMaxes();
    
    ret->push_back(p1);
    ret->push_back(p2);
    ret->push_back(vec3(p1.x, p1.y, p2.z));
    ret->push_back(vec3(p1.x, p2.y, p1.z));
    ret->push_back(vec3(p2.x, p1.y, p1.z));
    ret->push_back(vec3(p1.x, p2.y, p2.z));
    ret->push_back(vec3(p2.x, p1.y, p2.z));
    ret->push_back(vec3(p2.x, p2.y, p1.z));
    return ret;
}
