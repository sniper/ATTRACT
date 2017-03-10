//
//  GameObject.cpp
//  Lab1
//
//  Created by Cody Rhoads on 5/19/16.
//
//
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include "GameObject.hpp"
#include "Program.h"
#include "Shape.h"
#include "Material.hpp"
#include "BoundingSphere.hpp"
#include "MatrixStack.h"

using namespace std;
using namespace glm;

GameObject::GameObject() :
position(vec3(0.0f, 0.0f, 0.0f)),
oldPosition(vec3(0.0f, 0.0f, 0.0f)),
direction(vec3(0.0f, 0.0f, 0.0f)),
scale(vec3(1.0f, 1.0f, 1.0f)),
velocity(0.0f),
yRot(0.0f),
shape(make_shared<Shape>()),
material(make_shared<Material>()) {

}

GameObject::GameObject(const vec3 &position, const vec3 &direction,
        const vec3 &scale, float velocity,
        const shared_ptr<Shape> &shape,
        const shared_ptr<Material> &material) :
position(position),
oldPosition(position),
direction(direction),
scale(scale),
velocity(velocity),
shape(shape),
material(material) {

}

GameObject::~GameObject() {

}

bool GameObject::isCollidingWithBoundingSphere(const shared_ptr<BoundingSphere> &sphere) {
    return false;
}

bool GameObject::isCollidingWithBoundingBox(const shared_ptr<AABoundingBox> &box) {
    return false;
}

bool GameObject::isCollidingWithOtherObject(const shared_ptr<GameObject> &otherObj) {
    return false;
}

bool GameObject::isIntersectingWithLineSegment(const vec3 &start, const vec3 &end,
        vec3 *closestPointOfIntersection) {
    return false;
}

void GameObject::update(float dt) {
    oldPosition = position;
    position += direction * velocity * dt;
}

void GameObject::setPosition(vec3 pos) {
    position = pos;
}

void GameObject::draw(const shared_ptr<Program> &prog) {
    if (material != nullptr) {
        material->setUniforms(prog);
    }

    auto M = make_shared<MatrixStack>();
    M->translate(position);
    M->rotate(M_PI - atan2(direction.z, direction.x), vec3(0.0f, 1.0f, 0.0f));
    M->rotate(yRot,vec3(1,0,0));
    M->scale(scale);
    glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));

    shape->draw(prog);
}

void GameObject::resolveCollision(bool collidedWithPlayer) {
    position = oldPosition;
    //boundingSphere->updateCenter(position);

    if (collidedWithPlayer) {
        material = make_shared<Material>(vec3(0.2f, 0.2f, 0.2f), vec3(0.8f, 0.8f, 0.8f), vec3(1.0f, 0.9f, 0.8f), 200.0f);
        velocity = 0.0f;
    }
}
