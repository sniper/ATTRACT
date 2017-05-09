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
#include "Calculations.hpp"

using namespace std;
using namespace glm;

SpaceShipPart::SpaceShipPart() :
Cuboid(),
xOffsets(vector<float>()),
yOffset(0.0f),
zOffsets(vector<float>()),
currTime(0.0f),
collected(false),
spinning(false)
{
    
}

SpaceShipPart::SpaceShipPart(const vec3 &position, const vec3 &direction,
                             const vec3 &halfExtents, const vec3 &scale,
                             const shared_ptr<Shape> &shape,
                             const shared_ptr<Material> &material) :
Cuboid(position, direction, halfExtents, scale, 0.0f, shape, material, false),
partPieces(vector<shared_ptr<Shape>>()),
xOffsets(vector<float>()),
yOffset(0.0f),
zOffsets(vector<float>()),
currTime(0.0f),
collected(false),
spinning(false)
{
    
}

SpaceShipPart::SpaceShipPart(const vec3 &position, const vec3 &direction,
                             const vec3 &halfExtents, const vec3 &scale,
                             const vector<shared_ptr<Shape>> &pieces,
                             const shared_ptr<Material> &material) :
Cuboid(position, direction, halfExtents, scale, 0.0f, nullptr, material, false),
partPieces(pieces),
xOffsets(vector<float>()),
yOffset(0.0f),
zOffsets(vector<float>()),
currTime(0.0f),
collected(false),
spinning(false)
{
    for (unsigned int i = 0; i < partPieces.size(); i++) {
        xOffsets.push_back(0);
        zOffsets.push_back(0);
    }
}

bool SpaceShipPart::doneWinning() {
    return collected;
}

void SpaceShipPart::startWin() {
    if (!spinning) {
        spinning = true;
        spinTime = 0;
    }
    if (!expanding) {
        expanding = true;
        expandTime = 0;
        for (unsigned int i = 0; i < partPieces.size(); i++) {
            expandDirs.push_back(vec3(Calculations::randFloat(-0.5f, 0.5f),
                                      0.0f,
                                      Calculations::randFloat(-0.5f, 0.5f)));
        }
    }
}

void SpaceShipPart::update(float dt)
{
    currTime += dt;
    if (spinning) {
        spinTime += dt * 8;
        scale -= vec3(0.005, 0.005, 0.005);
        if (scale.x <= 0.01) {
            collected = true;
        }
    }
    if (expanding) {
        expandTime += dt * 8;
        if (expandTime < 1.0f) {
            for (unsigned int i = 0; i < partPieces.size(); i++) {
                xOffsets.at(i) += expandDirs.at(i).x;
                zOffsets.at(i) += expandDirs.at(i).z;
            }
        }
    }
    yOffset = 0.3 * sin(2 * currTime);
    
    if (currTime > 2*M_PI) {
        currTime -= 2*M_PI;
    }
    
    boundingBox->setPosition(vec3(position.x, position.y + yOffset, position.z));
}

void SpaceShipPart::draw(const std::shared_ptr<Program> &prog)
{
    if (partPieces.size() > 0) {
        for (unsigned int i = 0; i < partPieces.size(); i++) {
            auto M = make_shared<MatrixStack>();
            
            M->translate(vec3(position.x + xOffsets.at(i),
                              position.y + yOffset,
                              position.z + zOffsets.at(i)));
            
            if (spinning) {
                M->rotate(spinTime, vec3(0.0f, 1.0f, 0.0f));
            }
            M->scale(scale);
            glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
            
            partPieces.at(i)->draw(prog);
        }
    }
    else {
        auto M = make_shared<MatrixStack>();
        M->translate(vec3(position.x, position.y + yOffset, position.z));
        if (spinning) {
            M->rotate(spinTime, vec3(0.0f, 1.0f, 0.0f));
        }
        M->scale(scale);
        glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
        
        shape->draw(prog);
    }
}
