//
//  Camera.cpp
//  Lab1
//
//  Created by Cody Rhoads on 5/24/16.
//
//

#include <iostream>
#include <algorithm>
#include <memory>

#include "Camera.hpp"
#include "MatrixStack.h"
#include "BoundingSphere.hpp"
#include "GameObject.hpp"
#include "InputManager.hpp"
#include "GameManager.hpp"
#include "BulletManager.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <BulletCollision/Gimpact/btGImpactCollisionAlgorithm.h>

#define MOUSE_SENSITIVITY 0.07f
#define MOVEMENT_SPEED 3.0f
#define PITCH_CUTOFF 1.57
#define JUMP_CUTOFF 1
#define JUMP_VELOCITY 0.4f

using namespace std;
using namespace glm;

Camera::Camera() :
position(0.0f, 0.49f, 0.0f),
yaw(0.0f),
pitch(0.0f),
aspect(1.0f),
fovy(45.0f),
znear(0.1f),
zfar(1000.0f),
boundingSphere(make_shared<BoundingSphere>(position, 0.25f)),
lookingAtMagnet(false) {

}

Camera::Camera(int gridSize) :
position(0.0f, 0.49f, 0.0f),
yaw(0.0f),
pitch(0.0f),
aspect(1.0f),
fovy(45.0f),
znear(0.1f),
zfar(1000.0f),
boundingSphere(make_shared<BoundingSphere>(position, 0.25f)),
lookingAtMagnet(false) {

}

Camera::~Camera() {

}

void Camera::mouseMoved(double x, double y) {
    vec2 mouseCurr(x, y);
    vec2 dv = mouseCurr - mousePrev;

    if (dv[0] < 0.0f) {
        yaw -= MOUSE_SENSITIVITY;
    } else if (dv[0] > 0.0f) {
        yaw += MOUSE_SENSITIVITY;
    }

    if (dv[1] > 0.0f && pitch >= -PITCH_CUTOFF) {
        pitch -= MOUSE_SENSITIVITY;
    } else if (dv[1] < 0.0f && pitch <= PITCH_CUTOFF) {
        pitch += MOUSE_SENSITIVITY;
    }

    mousePrev = mouseCurr;
}

void Camera::interpretPressedKeys(const vector<char> &pressedKeys,
        shared_ptr<BulletManager> bullet,
        shared_ptr<FmodManager> fmod) {
    BulletObject *bulletCamObj = bullet->getBulletObject("cam");
    vec3 rot = vec3(cos(yaw), 0, cos((3.14 / 2) - yaw));
    vec3 lap = position + rot;
    vec3 forward = lap - position;
    forward.y = position.y;
    vec3 up;
    vec3 right;
    bool moving = false;

    if (cos(pitch) < 0) {
        up = vec3(0.0, -1.0, 0.0);
        right = cross(up, forward);
    } else {
        up = vec3(0.0, 1.0, 0.0);
        right = -1.0f * cross(up, forward);
    }
    btVector3 movement = btVector3(0.0f, bulletCamObj->getRigidBody()->getLinearVelocity().y(), 0.0f);
    oldPosition = position;

    /*debug stuff*/
    if (find(pressedKeys.begin(), pressedKeys.end(), 'p') != pressedKeys.end()) {
        bullet->setDebugFlag(!bullet->getDebugFlag());

    }

    if (find(pressedKeys.begin(), pressedKeys.end(), 'i') != pressedKeys.end()) {
        bullet->setDebugMode(btIDebugDraw::DBG_DrawWireframe);
    }
    if (find(pressedKeys.begin(), pressedKeys.end(), 'o') != pressedKeys.end()) {
        bullet->setDebugMode(btIDebugDraw::DBG_DrawAabb);
    }

    // Calculates the new camera position based on what keys are held down.
    // The keys that are held down are contained in the pressedKeys vector.
    if (find(pressedKeys.begin(), pressedKeys.end(), 'w') != pressedKeys.end()) {
        movement += (btVector3(forward.x, 0, forward.z) * MOVEMENT_SPEED);
        moving = true;

    }
    if (find(pressedKeys.begin(), pressedKeys.end(), 'a') != pressedKeys.end()) {
        movement += (btVector3(right.x, 0, right.z) * -MOVEMENT_SPEED);
        moving = true;
    }
    if (find(pressedKeys.begin(), pressedKeys.end(), 's') != pressedKeys.end()) {
        movement += (btVector3(forward.x, 0, forward.z) * -MOVEMENT_SPEED);
        moving = true;
    }
    if (find(pressedKeys.begin(), pressedKeys.end(), 'd') != pressedKeys.end()) {
        movement += (btVector3(right.x, 0, right.z) * MOVEMENT_SPEED);
        moving = true;
    }

    if (moving == true && !fmod->isPlaying("walking")) {

        fmod->playSound("walking", false);

    }

    if (find(pressedKeys.begin(), pressedKeys.end(), ' ') != pressedKeys.end()) {


        vec3 startV = bullet->getBulletObjectState("cam");
        btVector3 start = btVector3(startV.x, startV.y, startV.z);
        btVector3 end = btVector3(startV.x, startV.y - JUMP_CUTOFF, startV.z);

        // Finds the closest object from the start location to the end location.
        btCollisionWorld::ClosestRayResultCallback RayCB(start, end);
        bullet->getDynamicsWorld()->rayTest(start, end, RayCB);

        // Get the object that is hit.
        const btRigidBody *hitShape = (btRigidBody *) RayCB.m_collisionObject;

        /****** NOTE: IF AN OBJECT INTERRUPTS THE JUMPING PROCESS (GETS IN THE WAY) AND
         THE PLAYER HOLDS DOWN SPACE BAR, THE CHARACTER FLOATS ******/

        // Check if the ground was hit
        // TODO: check all objects which can be jumped off of
        std::map<std::string, BulletObject*> objects = bullet->getBulletObjects();
        for (auto iter = objects.begin(); iter != objects.end(); ++iter) {
            if (RayCB.hasHit() && hitShape == iter->second->getRigidBody()) {
                movement += btVector3(0, JUMP_VELOCITY, 0);



            }
        }
    }

    // ray test to see if you're in the air. If you are, don't have any friction
    vec3 startV = bullet->getBulletObjectState("cam");
    btVector3 start = btVector3(startV.x, startV.y, startV.z);
    btVector3 end = btVector3(startV.x, startV.y - 0.4, startV.z);

    // Finds the closest object from the start location to the end location.
    btCollisionWorld::ClosestRayResultCallback RayCB(start, end);
    bullet->getDynamicsWorld()->rayTest(start, end, RayCB);

    // Get the object that is hit.
    const btRigidBody *hitShape = (btRigidBody *) RayCB.m_collisionObject;

    bool jumping = true;
    // Check if the ground was hit
    // TODO: check all objects which can be jumped off of
    std::map<std::string, BulletObject*> objects = bullet->getBulletObjects();
    for (auto iter = objects.begin(); iter != objects.end(); ++iter) {
        if (RayCB.hasHit() && hitShape == iter->second->getRigidBody()) {
            jumping = false;

        }
    }
    if (jumping) {
        bulletCamObj->getRigidBody()->setFriction(0.0f);


    } else {
        bulletCamObj->getRigidBody()->setFriction(0.9f);


    }
    

    bulletCamObj->getRigidBody()->setLinearVelocity(movement);

    boundingSphere->updateCenter(position);
}

void Camera::applyProjectionMatrix(shared_ptr<MatrixStack> P) const {
    // Modify provided MatrixStack
    P->perspective(fovy, aspect, znear, zfar);
}

void Camera::applyViewMatrix(shared_ptr<MatrixStack> MV) const {
    vec3 rot = vec3(cos(pitch) * cos(yaw), sin(pitch), cos(pitch) * cos((3.14 / 2) - yaw));
    vec3 lap = position + rot;
    vec3 up;
    if (cos(pitch) < 0) {
        up = vec3(0.0, -1.0, 0.0);
    } else {
        up = vec3(0.0, 1.0, 0.0);
    }
    MV->lookAt(position, lap, up);
}

bool Camera::checkForCollision(const std::shared_ptr<GameObject> &otherObj) {
    return otherObj->isCollidingWithBoundingSphere(boundingSphere);
}

void Camera::resolveCollision() {
    position = oldPosition;
    boundingSphere->updateCenter(position);
}

void Camera::setPosition(vec3 inPos) {
    position = inPos;
}
