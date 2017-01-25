//
//  GameObject.cpp
//  Lab1
//
//  Created by Cody Rhoads on 5/19/16.
//
//
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "GameObject.hpp"
#include "Program.h"
#include "Shape.h"
#include "Material.hpp"
#include "BoundingSphere.hpp"
#include "MatrixStack.h"

using namespace std;
using namespace glm;

GameObject::GameObject() :
collected(false),
position(vec3(0.0f, 0.0f, 0.0f)),
oldPosition(vec3(0.0f, 0.0f, 0.0f)),
direction(vec3(0.0f, 0.0f, 0.0f)),
velocity(0.0f),
boundingSphere(make_shared<BoundingSphere>()),
shape(make_shared<Shape>()),
material(make_shared<Material>())
{

}

GameObject::GameObject(const glm::vec3 &position, const glm::vec3 &direction, float velocity,
                       const std::shared_ptr<BoundingSphere> &sphere,
                       const std::shared_ptr<Shape> &shape,
                       const std::shared_ptr<Material> &material) :
collected(false),
position(position),
oldPosition(position),
direction(direction),
velocity(velocity),
boundingSphere(sphere),
shape(shape),
material(material)
{
    
}

GameObject::~GameObject()
{
    
}

bool GameObject::isCollidingWithBoundingSphere(const std::shared_ptr<BoundingSphere> &otherSphere)
{
    return boundingSphere->isColliding(otherSphere);
}

bool GameObject::isCollidingWithOtherObject(const std::shared_ptr<GameObject> &otherObj)
{
    return otherObj->isCollidingWithBoundingSphere(boundingSphere);
}

void GameObject::update(float dt)
{
    oldPosition = position;
    position += direction * velocity * dt;
    boundingSphere->updateCenter(position);
}

void GameObject::draw(const shared_ptr<Program> &prog)
{
    material->setUniforms(prog);
    
    auto transMat = make_shared<MatrixStack>();
    transMat->translate(position);
    transMat->rotate(M_PI - atan2(direction[2], direction[0]), vec3(0.0f, 1.0f, 0.0f));
    glUniformMatrix4fv(prog->getUniform("objTransMatrix"), 1, GL_FALSE, value_ptr(transMat->topMatrix()));
    
    shape->draw(prog);
}

void GameObject::resolveCollision(bool collidedWithPlayer)
{
    position = oldPosition;
    boundingSphere->updateCenter(position);
    
    if (collidedWithPlayer) {
        collected = true;
        material = make_shared<Material>(vec3(0.2f, 0.2f, 0.2f), vec3(0.8f, 0.8f, 0.8f), vec3(1.0f, 0.9f, 0.8f), 200.0f);
        velocity = 0.0f;
    }
    else if (!collected) {
        direction *= -1;
    }
}
