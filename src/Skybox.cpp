//
//  Skybox.cpp
//  ATTRACT
//
//  Created by Cody Rhoads on 3/6/17.
//
//

#include "Skybox.hpp"
#include "Program.h"
#include "TextureCube.h"
#include "MatrixStack.h"
#include "Shape.h"
#include "GLSL.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;

Skybox::Skybox(const string &resourceDir, const shared_ptr<Shape> &skyShape) {
    prog = make_shared<Program>();
    prog->setShaderNames(resourceDir + "skyboxVert.glsl", resourceDir + "skyboxFrag.glsl");
    GLSL::checkError(GET_FILE_LINE);
    prog->setVerbose(false);
    prog->init();
    prog->addAttribute("aPos");
    prog->addAttribute("aNor");
    prog->addUniform("P");
    prog->addUniform("V");
    prog->addUniform("M");
    prog->addUniform("cubemap");
    GLSL::checkError();
    
    shape = skyShape;
    GLSL::checkError();
    
    tc = make_shared<TextureCube>();
    GLSL::checkError();
    tc->setFilenames(resourceDir + "skybox_negx.jpg", // negx
                     resourceDir + "skybox_posx.jpg", // posx
                     resourceDir + "skybox_negy.jpg", // negy
                     resourceDir + "skybox_posy.jpg", // posy
                     resourceDir + "skybox_negz.jpg", // negz
                     resourceDir + "skybox_posz.jpg"); // posz
    GLSL::checkError();
    tc->init();
    GLSL::checkError();
    tc->setUnit(0);
    GLSL::checkError();
}

Skybox::~Skybox()
{
    
}

void Skybox::render(shared_ptr<MatrixStack> &P, shared_ptr<MatrixStack> &V)
{
    shared_ptr<MatrixStack> M = make_shared<MatrixStack>();
    prog->bind();
    tc->bind(prog->getUniform("cubemap"));
    M->pushMatrix();
    M->loadIdentity();
    M->scale(500.0f);
    glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));
    glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(V->topMatrix()));
    glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
    shape->draw(prog);
    M->popMatrix();
    tc->unbind();
    prog->unbind();
}
