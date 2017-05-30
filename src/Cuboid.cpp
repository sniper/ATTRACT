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
position1(vec3(0.0f, 0.0f, 0.0f)),
position2(vec3(0.0f, 0.0f, 0.0f)),
boundingBox(make_shared<AABoundingBox>()),
magnetic(false),
waiting(0)
{
    
}

Cuboid::Cuboid(const vec3 &position, const vec3 &position2, const vec3 &direction,
               const vec3 &halfExtents, const vec3 &scale, float velocity,
               const shared_ptr<Shape> &shape,
               const shared_ptr<Material> &material, bool magnetic) :
GameObject(position, vec3(0, 0, 0), scale, velocity, shape, material),
position1(position),
position2(position2),
dir(direction),
boundingBox(make_shared<AABoundingBox>(position, vec3(halfExtents.x * scale.x,
                                                      halfExtents.y * scale.y,
                                                      halfExtents.z * scale.z))),
magnetic(magnetic),
waiting(0)
{
    
}

Cuboid::~Cuboid()
{
    
}

void Cuboid::update(float dt)
{
    GameObject::update(dt);
    boundingBox->setPosition(position);
    if (velocity == 0) {
        return;
    }
    if (waiting == 0.0f) {
        position += dir * velocity * dt;
        boundingBox->setPosition(position);
        float dist1 = distance(position, position1);
        float dist2 = distance(position, position2);
        float prevVelocity = velocity;
        if (dist1 <= 0.1) {
            velocity = abs(velocity);

        } else if (dist2 <= 0.1) {
            velocity = abs(velocity) * -1;
        }
        if (prevVelocity != velocity) {
            waiting = WAIT_TIME;
        }
    } else {
        waiting -= dt;
        if (waiting < 0) {
            waiting = 0;
        }
    }
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
    return boundingBox->isIntersectingWithRay(start, end, closestPointOfIntersection);
}

vector<vec3>* Cuboid::getAabbMinsMaxs() {
    vector<vec3> *temp = new vector<vec3>();
    temp->push_back(boundingBox->getMins());
    temp->push_back(boundingBox->getMaxes());
    return temp;
}
