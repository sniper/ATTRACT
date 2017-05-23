//
//  DebugDraw.cpp
//  ATTRACT
//
//  Created by Cody Rhoads on 5/17/17.
//
//

#include "DebugDraw.hpp"
#include "GLSL.h"
#include "Shape.h"
#include "GameObject.hpp"
#include "Material.hpp"
#include "Program.h"

using namespace std;
using namespace glm;

DebugDraw::DebugDraw() :
prog(nullptr),
P(mat4(1)),
V(mat4(1)),
sphere(nullptr)
{
    
}

DebugDraw::DebugDraw(const shared_ptr<Program> &prog, const shared_ptr<Shape> &sphere) :
prog(prog),
P(mat4(1)),
V(mat4(1)),
sphere(sphere)
{
    
}

void DebugDraw::drawPoint(const vec3 &position, const vec3 &color, const vec3 &scale)
{
    shared_ptr<Material> material = make_shared<Material>(color, vec3(0),
                                                          vec3(0), 0.0f);
    GameObject point = GameObject(position, vec3(0), scale, 0, sphere, material);
    
    prog->bind();
    glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, value_ptr(P));
    glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(V));
    point.draw(prog);
    prog->unbind();
}

void DebugDraw::setPV(const mat4 &newP, const mat4 &newV)
{
    P = newP;
    V = newV;
}
