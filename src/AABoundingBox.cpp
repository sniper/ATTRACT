//
//  AABoundingBox.cpp
//  ATTRACT
//
//  Created by Cody Rhoads on 1/29/17.
//
//

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>

#include "AABoundingBox.hpp"
#include "BoundingSphere.hpp"
#include "Calculations.hpp"

using namespace std;
using namespace glm;

AABoundingBox::AABoundingBox() :
position(vec3(0.0f, 0.0f, 0.0f)),
halfExtents(vec3(0.0f, 0.0f, 0.0f))
{
    
}

AABoundingBox::AABoundingBox(const vec3 &position, const vec3 &halfExtents) :
position(position),
halfExtents(halfExtents)
{
    
}



AABoundingBox::~AABoundingBox()
{
    
}

bool AABoundingBox::isCollidingWithSphere(const std::shared_ptr<BoundingSphere> &sphere)
{
    // Needs implementation at some point.
    return false;
}

bool AABoundingBox::isCollidingWithAABox(const std::shared_ptr<AABoundingBox> &box)
{
    if ((getMins().x <= box->getMaxes().x && getMaxes().x >= box->getMins().x) &&
        (getMins().y <= box->getMaxes().y && getMaxes().y >= box->getMins().y) &&
        (getMins().z <= box->getMaxes().z && getMaxes().z >= box->getMins().z)) {
        return true;
    }
    return false;
}

bool AABoundingBox::isIntersectingWithRay(const vec3 &start, const vec3 &end,
                                          vec3 *closestPointOfIntersection)
{
    float t, lineX, lineY, lineZ;
    float minX = position.x - halfExtents.x;
    float maxX = position.x + halfExtents.x;
    float minY = position.y - halfExtents.y;
    float maxY = position.y + halfExtents.y;
    float minZ = position.z - halfExtents.z;
    float maxZ = position.z + halfExtents.z;
    vec3 CPOI, currIntersection;
    bool foundIntersection = false;
    
    // If the ray starts inside the box, say there is an intersection.
    if (start.x > minX && start.x < maxX && start.y > minY && start.y < maxY
        && start.z > minZ && start.z < maxZ)
    {
        foundIntersection = true;
    }
    
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
        if (closestPointOfIntersection) {
            *closestPointOfIntersection = vec3(CPOI.x, CPOI.y, CPOI.z);
        }
        return true;
    }
    
    return false;
}

string AABoundingBox::getMinString() const
{
    string xCoord, yCoord, zCoord;
    
    xCoord = to_string(position.x - halfExtents.x);
    xCoord.erase(xCoord.find_last_not_of('0') + 1, string::npos);
    if (xCoord[xCoord.size()-1] == '.') {
        xCoord.append("0");
    }
    yCoord = to_string(position.y - halfExtents.y);
    yCoord.erase(yCoord.find_last_not_of('0') + 1, string::npos);
    if (yCoord[yCoord.size()-1] == '.') {
        yCoord.append("0");
    }
    zCoord = to_string(position.z - halfExtents.z);
    zCoord.erase(zCoord.find_last_not_of('0') + 1, string::npos);
    if (zCoord[zCoord.size()-1] == '.') {
        zCoord.append("0");
    }
    
    return "(" + xCoord + ", " + yCoord + ", " + zCoord + ")";
}

string AABoundingBox::getMaxString() const
{
    string xCoord, yCoord, zCoord;
    
    xCoord = to_string(position.x + halfExtents.x);
    xCoord.erase(xCoord.find_last_not_of('0') + 1, string::npos);
    if (xCoord[xCoord.size()-1] == '.') {
        xCoord.append("0");
    }
    yCoord = to_string(position.y + halfExtents.y);
    yCoord.erase(yCoord.find_last_not_of('0') + 1, string::npos);
    if (yCoord[yCoord.size()-1] == '.') {
        yCoord.append("0");
    }
    zCoord = to_string(position.z + halfExtents.z);
    zCoord.erase(zCoord.find_last_not_of('0') + 1, string::npos);
    if (zCoord[zCoord.size()-1] == '.') {
        zCoord.append("0");
    }
    
    return "(" + xCoord + ", " + yCoord + ", " + zCoord + ")";
}
