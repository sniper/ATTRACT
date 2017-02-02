//
//  BulletBox.cpp
//  ATTRACT
//
//  Created by Cody Rhoads on 1/30/17.
//
//

#include "BulletBox.hpp"

using namespace std;
using namespace glm;

BulletBox::BulletBox(vec3 loc, vec3 dims, vec3 scale, float mass) {
    btTransform trans; //position and rotation
    trans.setIdentity();
    trans.setOrigin(btVector3(loc.x, loc.y, loc.z));
    shape = new btBoxShape(btVector3(dims.x, dims.y, dims.z));
    shape->setLocalScaling(btVector3(scale.x,scale.y,scale.z));
    motionState = new btDefaultMotionState(trans);

    btVector3 inertia(0.0f, 0.0f, 0.0f);
    shape->calculateLocalInertia(mass, inertia);
    btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(mass, motionState, shape, inertia);
    rigidBodyCI.m_friction = 0.9f;
    rigidBodyCI.m_restitution = 0.0f;
    rigidBody = new btRigidBody(rigidBodyCI);
    rigidBody->setAngularFactor(btVector3(0, 0, 0));
    rigidBody->setActivationState(DISABLE_DEACTIVATION);
}

BulletBox::BulletBox(const BulletBox& orig) {
}

BulletBox::~BulletBox() {
}

