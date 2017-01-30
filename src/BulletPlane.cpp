/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   BulletPlane.cpp
 * Author: root
 * 
 * Created on January 30, 2017, 10:42 AM
 */

#include "BulletPlane.h"

BulletPlane::BulletPlane(double x, double y, double z) {
    btTransform trans; //position and rotation
    trans.setIdentity();
    trans.setOrigin(btVector3(x, y, z));
    
    
    shape = new btStaticPlaneShape(btVector3(0, 1, 0), 0);
    motionState = new btDefaultMotionState(
            trans);
    btRigidBody::btRigidBodyConstructionInfo
    rigidBodyCI(0, motionState, shape, btVector3(0, 0, 0));
    rigidBodyCI.m_friction = 0.9f;
    rigidBodyCI.m_restitution = 0.5f;
    rigidBody = new btRigidBody(rigidBodyCI);
}

BulletPlane::BulletPlane(const BulletPlane& orig) {
}

BulletPlane::~BulletPlane() {
}

