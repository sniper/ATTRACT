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
#define PITCH_CUTOFF 1.57

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
boundingSphere(make_shared<BoundingSphere>(position, 0.25f))
{

}

Camera::Camera(int gridSize) :
position(0.0f, 0.49f, 0.0f),
yaw(0.0f),
pitch(0.0f),
aspect(1.0f),
fovy(45.0f),
znear(0.1f),
zfar(1000.0f),
boundingSphere(make_shared<BoundingSphere>(position, 0.25f))
{

}

Camera::~Camera()
{

}

void Camera::mouseMoved(double x, double y)
{
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

vec3 Camera::getForward() {
    vec3 rot = vec3(cos(pitch) * cos(yaw), sin(pitch), cos(pitch) * cos((3.14 / 2) - yaw));
    vec3 lap = position + rot;
    vec3 forward = normalize(lap - position);
    return forward;
}

void Camera::interpretPressedKeys(const vector<char> &pressedKeys)
{
    vec3 rot = vec3(cos(pitch) * cos(yaw), sin(pitch), cos(pitch) * cos((3.14 / 2) - yaw));
    vec3 lap = position + rot;
    vec3 forward = lap - position;
    forward.y = 0;
    forward = normalize(forward);
    vec3 up;
    vec3 right;
    if (cos(pitch) < 0) {
        up = vec3(0.0, -1.0, 0.0);
    } else {
        up = vec3(0.0, 1.0, 0.0);
    }
    right = -1.0f * cross(up, forward);
    forward = normalize(lap - position);
    oldPosition = position;

    // Calculates the new camera position based on what keys are held down.
    // The keys that are held down are contained in the pressedKeys vector.
    if (find(pressedKeys.begin(), pressedKeys.end(), '^') != pressedKeys.end()) {
        forward *= 2;
        right *= 2;
    }
    if (find(pressedKeys.begin(), pressedKeys.end(), 'w') != pressedKeys.end()) {
        position += MOVEMENT_SPEED * forward;
    }
    if (find(pressedKeys.begin(), pressedKeys.end(), 'a') != pressedKeys.end()) {
        position += MOVEMENT_SPEED * -right;
    }
    if (find(pressedKeys.begin(), pressedKeys.end(), 's') != pressedKeys.end()) {
        position += MOVEMENT_SPEED * -forward;
    }
    if (find(pressedKeys.begin(), pressedKeys.end(), 'd') != pressedKeys.end()) {
        position += MOVEMENT_SPEED * right;
    }

    if (position[1] < 0.49f) {
        //position[1] = 0.49f;
    }
    boundingSphere->updateCenter(position);
}

void Camera::applyProjectionMatrix(shared_ptr<MatrixStack> P) const
{
    // Modify provided MatrixStack
    P->perspective(fovy, aspect, znear, zfar);
}

void Camera::applyViewMatrix(shared_ptr<MatrixStack> MV) const
{
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

bool Camera::checkForCollision(const std::shared_ptr<GameObject> &otherObj)
{
    //return otherObj->isCollidingWithBoundingSphere(boundingSphere);
    return false;
}

void Camera::resolveCollision()
{
    position = oldPosition;
    boundingSphere->updateCenter(position);
}

