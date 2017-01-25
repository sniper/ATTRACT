//
//  Material.cpp
//  Lab1
//
//  Created by Cody Rhoads on 5/6/16.
//
//

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Material.hpp"
#include "Program.h"

using namespace std;
using namespace glm;

Material::Material() :
ka(vec3(0.0f, 0.0f, 0.0f)),
kd(vec3(0.0f, 0.0f, 0.0f)),
ks(vec3(0.0f, 0.0f, 0.0f)),
s(0.0f)
{
    
}

Material::Material(const vec3 &ka, const vec3 &kd, const vec3 &ks, float s) :
    ka(ka),
    kd(kd),
    ks(ks),
    s(s)
{
    
}

Material::~Material()
{
    
}

void Material::setUniforms(const shared_ptr<Program> &prog) {
    glUniform3f(prog->getUniform("ka"), ka[0], ka[1], ka[2]);
    glUniform3f(prog->getUniform("kd"), kd[0], kd[1], kd[2]);
    glUniform3f(prog->getUniform("ks"), ks[0], ks[1], ks[2]);
    glUniform1f(prog->getUniform("s"), s);
}
