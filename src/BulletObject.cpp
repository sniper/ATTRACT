/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   BulletObject.cpp
 * Author: root
 * 
 * Created on January 30, 2017, 10:36 AM
 */

#include "BulletObject.h"

BulletObject::BulletObject() {
    
}

BulletObject::BulletObject(const BulletObject& orig) {
}

BulletObject::~BulletObject() {
    delete rigidBody;
    delete motionState;
    delete shape;
}

btRigidBody* BulletObject::getRigidBody() {
    return rigidBody;
}
