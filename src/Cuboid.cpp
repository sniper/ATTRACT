//
//  Cuboid.cpp
//  ATTRACT
//
//  Created by Cody Rhoads on 2/6/17.
//
//
#include <iostream>
#include <vector>
#include "Cuboid.hpp"
#include "AABoundingBox.hpp"
#include "Calculations.hpp"

using namespace std;
using namespace glm;

Cuboid::Cuboid() :
GameObject(),
boundingBox(make_shared<AABoundingBox>()),
magnetic(false)
{
    
}

Cuboid::Cuboid(const vec3 &position, const vec3 &direction,
               const vec3 &halfExtents, const vec3 &scale, float velocity,
               const shared_ptr<Shape> &shape,
               const shared_ptr<Material> &material, bool magnetic) :
GameObject(position, direction, scale, velocity, shape, material),
boundingBox(make_shared<AABoundingBox>(position, vec3(halfExtents.x * scale.x,
                                                      halfExtents.y * scale.y,
                                                      halfExtents.z * scale.z))),
magnetic(magnetic)
{
    
}

Cuboid::~Cuboid()
{
    
}

void Cuboid::update(float dt)
{
    GameObject::update(dt);
    boundingBox->setPosition(position);
}

bool Cuboid::isCollidingWithBoundingSphere(const std::shared_ptr<BoundingSphere> &sphere)
{
    return boundingBox->isCollidingWithSphere(sphere);
}

bool Cuboid::isCollidingWithBoundingBox(const std::shared_ptr<AABoundingBox> &box)
{
    return boundingBox->isCollidingWithAABox(box);
}

bool Cuboid::isCollidingWithOtherObject(const std::shared_ptr<GameObject> &otherObj)
{
    // Not sure if we'll need this.
    return false;
}

bool Cuboid::isIntersectingWithLineSegment(const vec3 &start, const vec3 &end,
                                           vec3 *closestPointOfIntersection)
{
    float t, lineX, lineY, lineZ;
    float minX = position.x - boundingBox->getXHalfExtent();
    float maxX = position.x + boundingBox->getXHalfExtent();
    float minY = position.y - boundingBox->getYHalfExtent();
    float maxY = position.y + boundingBox->getYHalfExtent();
    float minZ = position.z - boundingBox->getZHalfExtent();
    float maxZ = position.z + boundingBox->getZHalfExtent();
    vec3 CPOI, currIntersection;
    bool foundIntersection = false;
    
    /** Checking for intersection with min X value of cuboid. **/
    // The equation for the line's x value at any value t is as follows:
    // x = start.x + (end.x - start.x) * t
    // By setting this equal to the minimum X value and solving for t, we can
    // find when the line intersects that x value.
    t = (minX - start.x)/(end.x - start.x);
    // We can then compute the line's y and z values at that t value.
    lineY = start.y + (end.y - start.y) * t;
    lineZ = start.z + (end.z - start.z) * t;
    // If the line is within the Y and Z bounds, then the line is intersecting
    // the cuboid. Since this is a line segment, we also need to check if the
    // cuboid is in the range between the start and end of the line segment.
    if (lineY >= minY && lineY <= maxY && lineZ >= minZ && lineZ <= maxZ &&
        lineY >= std::min(start.y, end.y) && lineY <= std::max(start.y, end.y) &&
        lineZ >= std::min(start.z, end.z) && lineZ <= std::max(start.z, end.z)) {
        
        currIntersection = vec3(minX, lineY, lineZ);
        // If this is the first intersection found, or if this is the closest
        // point of intersection relative to the "start", keep track of the point.
        if (!foundIntersection ||
            Calculations::findDistance(currIntersection, start) <
            Calculations::findDistance(CPOI, start)) {
            CPOI = currIntersection;
        }
        foundIntersection = true;
    }
    
    /** Checking for intersection with max X value of cuboid. **/
    t = (maxX - start.x)/(end.x - start.x);
    lineY = start.y + (end.y - start.y) * t;
    lineZ = start.z + (end.z - start.z) * t;
    if (lineY >= minY && lineY <= maxY && lineZ >= minZ && lineZ <= maxZ &&
        lineY >= std::min(start.y, end.y) && lineY <= std::max(start.y, end.y) &&
        lineZ >= std::min(start.z, end.z) && lineZ <= std::max(start.z, end.z)) {
        
        currIntersection = vec3(maxX, lineY, lineZ);
        if (!foundIntersection ||
            Calculations::findDistance(currIntersection, start) <
            Calculations::findDistance(CPOI, start)) {
            CPOI = currIntersection;
        }
        foundIntersection = true;
    }
    
    /** Checking for intersection with min Y value of cuboid. **/
    t = (minY - start.y)/(end.y - start.y);
    lineX = start.x + (end.x - start.x) * t;
    lineZ = start.z + (end.z - start.z) * t;
    if (lineX >= minX && lineX <= maxX && lineZ >= minZ && lineZ <= maxZ &&
        lineX >= std::min(start.x, end.x) && lineX <= std::max(start.x, end.x) &&
        lineZ >= std::min(start.z, end.z) && lineZ <= std::max(start.z, end.z)) {
        
        currIntersection = vec3(lineX, minY, lineZ);
        if (!foundIntersection ||
            Calculations::findDistance(currIntersection, start) <
            Calculations::findDistance(CPOI, start)) {
            CPOI = currIntersection;
        }
        foundIntersection = true;
    }
    
    /** Checking for intersection with max Y value of cuboid. **/
    t = (maxY - start.y)/(end.y - start.y);
    lineX = start.x + (end.x - start.x) * t;
    lineZ = start.z + (end.z - start.z) * t;
    if (lineX >= minX && lineX <= maxX && lineZ >= minZ && lineZ <= maxZ &&
        lineX >= std::min(start.x, end.x) && lineX <= std::max(start.x, end.x) &&
        lineZ >= std::min(start.z, end.z) && lineZ <= std::max(start.z, end.z)) {
        
        currIntersection = vec3(lineX, maxY, lineZ);
        if (!foundIntersection ||
            Calculations::findDistance(currIntersection, start) <
            Calculations::findDistance(CPOI, start)) {
            CPOI = currIntersection;
        }
        foundIntersection = true;
    }
    
    /** Checking for intersection with min Z value of cuboid. **/
    t = (minZ - start.z)/(end.z - start.z);
    lineX = start.x + (end.x - start.x) * t;
    lineY = start.y + (end.y - start.y) * t;
    if (lineX >= minX && lineX <= maxX && lineY >= minY && lineY <= maxY &&
        lineX >= std::min(start.x, end.x) && lineX <= std::max(start.x, end.x) &&
        lineY >= std::min(start.y, end.y) && lineY <= std::max(start.y, end.y)) {
        
        currIntersection = vec3(lineX, lineY, minZ);
        if (!foundIntersection ||
            Calculations::findDistance(currIntersection, start) <
            Calculations::findDistance(CPOI, start)) {
            CPOI = currIntersection;
        }
        foundIntersection = true;
    }
    
    /** Checking for intersection with max Z value of cuboid. **/
    t = (maxZ - start.z)/(end.z - start.z);
    lineX = start.x + (end.x - start.x) * t;
    lineY = start.y + (end.y - start.y) * t;
    if (lineX >= minX && lineX <= maxX && lineY >= minY && lineY <= maxY &&
        lineX >= std::min(start.x, end.x) && lineX <= std::max(start.x, end.x) &&
        lineY >= std::min(start.y, end.y) && lineY <= std::max(start.y, end.y)) {
        
        currIntersection = vec3(lineX, lineY, maxZ);
        if (!foundIntersection ||
            Calculations::findDistance(currIntersection, start) <
            Calculations::findDistance(CPOI, start)) {
            CPOI = currIntersection;
        }
        foundIntersection = true;
    }
    
    if (foundIntersection) {
        *closestPointOfIntersection = vec3(CPOI.x, CPOI.y, CPOI.z);
        return true;
    }
    
    return false;
}

std::vector<glm::vec3>* Cuboid::getAabbMinsMaxs() {
    std::vector<glm::vec3> *temp = new vector<glm::vec3>();
    temp->push_back(boundingBox->getMins());
    temp->push_back(boundingBox->getMaxes());
    return temp;
}
