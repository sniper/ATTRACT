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

BulletBox::BulletBox(double x, double y, double z, vec3 dims) {
    btTransform trans; //position and rotation
    trans.setIdentity();
    trans.setOrigin(btVector3(x, y, z));
    shape = new btBoxShape(btVector3(dims.x, dims.y, dims.z));
    motionState = new btDefaultMotionState(trans);
    btScalar mass = 1.0f;
    btVector3 inertia(0.0f, 0.0f, 0.0f);
    shape->calculateLocalInertia(mass, inertia);
    btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(mass, motionState, shape, inertia);
    rigidBodyCI.m_friction = 0.9f;
    rigidBodyCI.m_angularDamping = 1000.0f;
    rigidBodyCI.m_restitution = 0.0f;
    rigidBody = new btRigidBody(rigidBodyCI);
    rigidBody->setActivationState(DISABLE_DEACTIVATION);
}

BulletBox::BulletBox(const BulletBox& orig) {
}

BulletBox::~BulletBox() {
}

