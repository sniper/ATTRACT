/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   BulletObject.h
 * Author: root
 *
 * Created on January 30, 2017, 10:36 AM
 */

#ifndef BULLETOBJECT_H
#define BULLETOBJECT_H

#include <iostream>
#include <btBulletDynamicsCommon.h>
#include <BulletCollision/Gimpact/btGImpactCollisionAlgorithm.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;
using namespace glm;

class BulletObject {
public:
    BulletObject();
    BulletObject(const BulletObject& orig);
    virtual ~BulletObject();
    btRigidBody *getRigidBody();
    
protected:
    /*Collision shapes*/
    btCollisionShape* shape;

    /*motion states */
    btDefaultMotionState* motionState;

    /*rigid bodies*/
    btRigidBody* rigidBody;
};

#endif /* BULLETOBJECT_H */

