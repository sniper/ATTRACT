/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   BulletSphere.cpp
 * Author: root
 * 
 * Created on January 30, 2017, 12:20 PM
 */

#include "BulletSphere.h"

BulletSphere::BulletSphere(double x, double y, double z, double radius) {
    btTransform trans; //position and rotation
    trans.setIdentity();
    trans.setOrigin(btVector3(x, y, z));
    shape = new btSphereShape(radius);
    motionState = new btDefaultMotionState(trans);
    btScalar mass = 1.0f;
    btVector3 sphereInertia(0.0f, 0.0f, 0.0f);
    shape->calculateLocalInertia(mass, sphereInertia);
    btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(mass, motionState, shape, sphereInertia);
    rigidBodyCI.m_friction = 0.9f;
    rigidBodyCI.m_restitution = 0.5f;
    rigidBodyCI.m_rollingFriction = 0.1f;
    rigidBody = new btRigidBody(rigidBodyCI);
}

BulletSphere::BulletSphere(const BulletSphere& orig) {
}

BulletSphere::~BulletSphere() {
}

