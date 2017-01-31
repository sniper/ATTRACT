//
//  Camera.cpp
//  Lab1
//
//  Created by Cody Rhoads on 5/24/16.
//
//

#include <iostream>
#include <algorithm>

#include "Camera.hpp"
#include "MatrixStack.h"
#include "BoundingSphere.hpp"
#include "GameObject.hpp"
#include "InputManager.hpp"
#include "GameManager.hpp"
#include "BulletManager.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <BulletCollision/Gimpact/btGImpactCollisionAlgorithm.h>

#define MOUSE_SENSITIVITY 0.07f
#define MOVEMENT_SPEED 3.0f
#define JUMP_CUTOFF 1
#define JUMP_VELOCITY 0.6f

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
boundingSphere(make_shared<BoundingSphere>(position, 0.25f)) {

}

Camera::Camera(int gridSize) :
position(0.0f, 0.49f, 0.0f),
yaw(0.0f),
pitch(0.0f),
aspect(1.0f),
fovy(45.0f),
znear(0.1f),
zfar(1000.0f),
boundingSphere(make_shared<BoundingSphere>(position, 0.25f)) {

}

Camera::~Camera() {

}

void Camera::mouseMoved(double x, double y) {
    vec2 mouseCurr(x, y);
    vec2 dv = mouseCurr - mousePrev;

    if (dv[0] < 0.0f) {
        yaw += MOUSE_SENSITIVITY;
    } else if (dv[0] > 0.0f) {
        yaw -= MOUSE_SENSITIVITY;
    }

    if (dv[1] > 0.0f && pitch > -1) {
        pitch -= MOUSE_SENSITIVITY;
    } else if (dv[1] < 0.0f && pitch < 1) {
        pitch += MOUSE_SENSITIVITY;
    }

    mousePrev = mouseCurr;
}

void Camera::interpretPressedKeys(const vector<char> &pressedKeys, BulletManager *bullet) {
    BulletObject *bulletCamObj = bullet->getBulletObject("cam");
    vec3 forward = vec3(sin(yaw), 0.0f, cos(yaw));
    vec3 right = vec3(-cos(yaw), 0.0f, sin(yaw));
    btVector3 movement = btVector3(0.0f, bulletCamObj->getRigidBody()->getLinearVelocity().y(), 0.0f);
    oldPosition = position;

    // Calculates the new camera position based on what keys are held down.
    // The keys that are held down are contained in the pressedKeys vector.
    if (find(pressedKeys.begin(), pressedKeys.end(), 'w') != pressedKeys.end()) {
        position += MOVEMENT_SPEED * forward;
        movement += (btVector3(forward.x, 0, forward.z) * MOVEMENT_SPEED);
    }
    if (find(pressedKeys.begin(), pressedKeys.end(), 'a') != pressedKeys.end()) {
        position -= MOVEMENT_SPEED * right;
        movement += (btVector3(right.x, 0, right.z) * -MOVEMENT_SPEED);
    }
    if (find(pressedKeys.begin(), pressedKeys.end(), 's') != pressedKeys.end()) {
        position -= MOVEMENT_SPEED * forward;
        movement += (btVector3(forward.x, 0, forward.z) * -MOVEMENT_SPEED);
    }
    if (find(pressedKeys.begin(), pressedKeys.end(), 'd') != pressedKeys.end()) {
        position += MOVEMENT_SPEED * right;
        movement += (btVector3(right.x, 0, right.z) * MOVEMENT_SPEED);
    }
    if (find(pressedKeys.begin(), pressedKeys.end(), ' ') != pressedKeys.end()) {
        vec3 startV = bullet->getBulletObjectState("cam");
        btVector3 start = btVector3(startV.x, startV.y, startV.z);
        btVector3 end = btVector3(startV.x, startV.y - JUMP_CUTOFF, startV.z);

        // Finds the closest object from the start location to the end location.
        btCollisionWorld::ClosestRayResultCallback RayCallback(start, end);
        bullet->getDynamicsWorld()->rayTest(start, end, RayCallback);

        // Get the object that is hit.
        const btRigidBody *hitShape = (btRigidBody *) RayCallback.m_collisionObject;

        // Check if the ground was hit
        // TODO: check all objects which can be jumped off of
        if (RayCallback.hasHit() && hitShape == bullet->getBulletObject("ground")->getRigidBody()) {
            movement += btVector3(0, JUMP_VELOCITY, 0);
        }
    }
    
    bulletCamObj->getRigidBody()->setLinearVelocity(movement);

    position[1] = 0.49f;

    boundingSphere->updateCenter(position);
}

void Camera::applyProjectionMatrix(shared_ptr<MatrixStack> P) const {
    // Modify provided MatrixStack
    P->perspective(fovy, aspect, znear, zfar);
}

void Camera::applyViewMatrix(shared_ptr<MatrixStack> MV) const {
    vec3 f = vec3(sin(yaw), sin(pitch), cos(yaw));
    MV->lookAt(position, position + f, vec3(0.0f, 1.0f, 0.0f));
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
