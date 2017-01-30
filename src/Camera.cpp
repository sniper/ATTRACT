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
boundingSphere(make_shared<BoundingSphere>(position, 0.25f)),
inputLockFlag(false) {

}

Camera::Camera(int gridSize, shared_ptr<BulletManager> bm) :
position(0.0f, 1.0f, 0.0f),
yaw(0.0f),
pitch(0.0f),
aspect(1.0f),
fovy(45.0f),
znear(0.1f),
zfar(1000.0f),
boundingSphere(make_shared<BoundingSphere>(position, 0.25f)),
inputLockFlag(false),
bullet(bm) {

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

void Camera::interpretPressedKeys(const vector<char> &pressedKeys) {
    vec3 forward = vec3(sin(yaw), 0.0f, cos(yaw));
    vec3 right = vec3(-cos(yaw), 0.0f, sin(yaw));
    oldPosition = position;



    // Calculates the new camera position based on what keys are held down.
    // The keys that are held down are contained in the pressedKeys vector.
    //lock is for jumping- while jumping you wont be abe to affect anything
    if (!inputLockFlag) {
        int key = 0;
        if (find(pressedKeys.begin(), pressedKeys.end(), 'w') != pressedKeys.end()) {
            position += MOVEMENT_SPEED * forward;
            key +='w';
        }
        if (find(pressedKeys.begin(), pressedKeys.end(), 'a') != pressedKeys.end()) {
            position -= MOVEMENT_SPEED * right;
            key +='a';
        }
        if (find(pressedKeys.begin(), pressedKeys.end(), 's') != pressedKeys.end()) {
            position -= MOVEMENT_SPEED * forward;
            key +='s';
        }
        if (find(pressedKeys.begin(), pressedKeys.end(), 'd') != pressedKeys.end()) {
            position += MOVEMENT_SPEED * right;
            key +='d';
        }

        /*jump check*/

        if (find(pressedKeys.begin(), pressedKeys.end(), ' ') != pressedKeys.end()) {
            cerr << "JUMP" << endl;
            inputLockFlag = true;

            bullet->setStepFlag(true);

            bullet->createSphere(position, 2);
            bullet->setJumpDirection(getDirection(), key);


        }

        position[1] = 1.0f;

    }



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
