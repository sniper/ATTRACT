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
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define MOUSE_SENSITIVITY 0.07f
#define MOVEMENT_SPEED 0.05f

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

void Camera::interpretPressedKeys(const vector<char> &pressedKeys, BulletObject *bulletCamObj) {
    vec3 forward = vec3(sin(yaw), sin(pitch), cos(yaw));
    vec3 right = vec3(-cos(yaw), 0.0f, sin(yaw));
    oldPosition = position;

    // Calculates the new camera position based on what keys are held down.
    // The keys that are held down are contained in the pressedKeys vector.
    if (find(pressedKeys.begin(), pressedKeys.end(), 'w') != pressedKeys.end()) {
        position += MOVEMENT_SPEED * forward;
        bulletCamObj->getRigidBody()->applyCentralForce(btVector3(forward.x, forward.y, forward.z)*10   );
    }
    if (find(pressedKeys.begin(), pressedKeys.end(), 'a') != pressedKeys.end()) {
        position -= MOVEMENT_SPEED * right;
         bulletCamObj->getRigidBody()->applyCentralForce(btVector3(right.x, right.y, right.z)*-10   );
    }
    if (find(pressedKeys.begin(), pressedKeys.end(), 's') != pressedKeys.end()) {
        position -= MOVEMENT_SPEED * forward;
         bulletCamObj->getRigidBody()->applyCentralForce(btVector3(forward.x, forward.y, forward.z)*-10   );
    }
    if (find(pressedKeys.begin(), pressedKeys.end(), 'd') != pressedKeys.end()) {
        position += MOVEMENT_SPEED * right;
         bulletCamObj->getRigidBody()->applyCentralForce(btVector3(right.x, right.y, right.z)*10   );
    }


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