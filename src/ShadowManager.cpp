//
//  ShadowManager.cpp
//  ATTRACT
//
//  Created by Cody Rhoads on 3/10/17.
//
//

#include "ShadowManager.hpp"
#include "GLSL.h"

ShadowManager::ShadowManager()
{
    
}

ShadowManager::~ShadowManager()
{
    
}

void ShadowManager::init()
{
    // Set up for debugging.
    glGenVertexArrays(1, &quad_VertexArrayID);
    glBindVertexArray(quad_VertexArrayID);
    
    static const GLfloat g_quad_vertex_buffer_data[] = {
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        1.0f,  1.0f, 0.0f,
    };
    
    glGenBuffers(1, &quad_vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data), g_quad_vertex_buffer_data, GL_STATIC_DRAW);
    
    //generate the FBO for the shadow depth
    glGenFramebuffers(1, &depthMapFBO);
    GLSL::checkError();
    
    //generate the texture
    glGenTextures(sizeof(depthMap)/sizeof(*depthMap), depthMap);
    GLSL::checkError();
    
    //generate a depth map for every cascade
    for (unsigned int i = 0; i < sizeof(depthMap)/sizeof(*depthMap); i++) {
        glBindTexture(GL_TEXTURE_2D, depthMap[i]);
        GLSL::checkError();
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT,
                     0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        GLSL::checkError();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        GLSL::checkError();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        GLSL::checkError();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        GLSL::checkError();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        GLSL::checkError();
    }
    GLSL::checkError();
    
    //bind with framebuffer's depth buffer
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap[0], 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    GLSL::checkError();
}

void ShadowManager::bindFramebuffer(unsigned int cascadeIndex)
{
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap[cascadeIndex], 0);
    glClear(GL_DEPTH_BUFFER_BIT);
}

void ShadowManager::unbindFramebuffer()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShadowManager::bind(GLint handle[])
{
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, depthMap[0]);
    glUniform1i(handle[0], unit);
    
//    glActiveTexture(GL_TEXTURE0 + unit + 1);
//    glBindTexture(GL_TEXTURE_2D, depthMap[1]);
//    glUniform1i(handle[1], unit + 1);
//    
//    glActiveTexture(GL_TEXTURE0 + unit + 2);
//    glBindTexture(GL_TEXTURE_2D, depthMap[2]);
//    glUniform1i(handle[2], unit + 2);
}

void ShadowManager::unbind()
{
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, 0);
    
//    glActiveTexture(GL_TEXTURE0 + unit + 1);
//    glBindTexture(GL_TEXTURE_2D, 0);
//    
//    glActiveTexture(GL_TEXTURE0 + unit + 2);
//    glBindTexture(GL_TEXTURE_2D, 0);
}

void ShadowManager::drawDebug()
{
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *) 0);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(0);
}
