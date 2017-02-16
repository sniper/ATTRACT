//
//  SpaceShipPart.cpp
//  ATTRACT
//
//  Created by Cody Rhoads on 2/15/17.
//
//

#include <iostream>

#include "SpaceShipPart.hpp"
#include "Material.hpp"
#include "MatrixStack.h"
#include "Program.h"
#include "MatrixStack.h"
#include "Shape.h"

using namespace std;
using namespace glm;

SpaceShipPart::SpaceShipPart() :
Cuboid(),
yOffset(0.0f),
currTime(0.0f),
collected(false)
{
    
}

SpaceShipPart::SpaceShipPart(const vec3 &position, const vec3 &direction,
                             const vec3 &halfExtents, const vec3 &scale,
                             const shared_ptr<Shape> &shape,
                             const shared_ptr<Material> &material) :
Cuboid(position, direction, halfExtents, scale, 0.0f, shape, material, false),
yOffset(0.0f),
currTime(0.0f),
collected(false)
{
    
}

SpaceShipPart::~SpaceShipPart()
{
    
}

void SpaceShipPart::update(float dt)
{
    currTime += dt;
    yOffset = 0.3 * sin(2 * currTime);
    
    if (currTime > 2*M_PI) {
        currTime -= 2*M_PI;
    }
    
    boundingBox->setPosition(vec3(position.x, position.y + yOffset, position.z));
}

void SpaceShipPart::draw(const std::shared_ptr<Program> &prog)
{
    material->setUniforms(prog);
    
    auto transMat = make_shared<MatrixStack>();
    transMat->translate(vec3(position.x, position.y + yOffset, position.z));
    transMat->rotate(M_PI - atan2(direction.z, direction.x), vec3(0.0f, 1.0f, 0.0f));
    transMat->scale(scale);
    glUniformMatrix4fv(prog->getUniform("objTransMatrix"), 1, GL_FALSE, value_ptr(transMat->topMatrix()));
    
    shape->draw(prog);
}
