//
//  ShadowManager.cpp
//  ATTRACT
//
//  Created by Cody Rhoads on 3/6/17.
//
//

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "ShadowManager.hpp"
#include "Program.h"

using namespace std;
using namespace glm;

ShadowManager::ShadowManager(const string &RESOURCE_DIR)
{
    depthProg = make_shared<Program>();
    depthProg->setVerbose(true);
    depthProg->setShaderNames(RESOURCE_DIR + "shadowDepthVert.glsl",
                              RESOURCE_DIR + "shadowDepthFrag.glsl");
    depthProg->init();
    
    /// Add uniform and attributes to each of the programs
    depthProg->addUniform("LP");
    depthProg->addUniform("LV");
    depthProg->addUniform("M");
    depthProg->addAttribute("aPos");
    //un-needed, but easier then modifying shape
    depthProg->addAttribute("aNor");
    depthProg->addAttribute("aTex");
    
    //generate the FBO for the shadow depth
    glGenFramebuffers(1, &depthMapFBO);
    
    //generate the texture
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, S_WIDTH, S_HEIGHT,
                 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    //bind with framebuffer's depth buffer
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

ShadowManager::~ShadowManager()
{
    
}

void ShadowManager::bindDepthMap(const vec3 &lightDir)
{
    //set up light's depth map
    glViewport(0, 0, S_WIDTH, S_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    glCullFace(GL_FRONT);
    
    //set up shadow shader
    depthProg->bind();
    mat4 LO = SetOrthoMatrix();
    mat4 LV = SetLightView(lightDir, vec3(0, 0, 0), vec3(0, 1, 0));
    LSpace = LO*LV;
}

void ShadowManager::unbindDepthMap()
{
    depthProg->unbind();
    glCullFace(GL_BACK);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShadowManager::addShadowsToScene(const shared_ptr<Program> &shader)
{
    glActiveTexture(GL_TEXTURE1);
   	glBindTexture(GL_TEXTURE_2D, depthMap);
    glUniform1i(shader->getUniform("shadowDepth"), 1);
    glUniformMatrix4fv(shader->getUniform("LS"), 1, GL_FALSE, value_ptr(LSpace));
}

mat4 ShadowManager::SetOrthoMatrix()
{
    mat4 OP = glm::ortho(-10.0, 10.0, -10.0, 10.0, 0.1, 30.0);
    glUniformMatrix4fv(depthProg->getUniform("LP"), 1, GL_FALSE, value_ptr(OP));
    return OP;
}

mat4 ShadowManager::SetLightView(vec3 pos, vec3 LA, vec3 up)
{
    mat4 Cam = glm::lookAt(pos, LA, up);
    glUniformMatrix4fv(depthProg->getUniform("LV"), 1, GL_FALSE, value_ptr(Cam));
    return Cam;
}
