/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   GuiManager.cpp
 * Author: root
 * 
 * Created on February 9, 2017, 3:36 PM
 */

#include "GuiManager.hpp"

using namespace std;

GuiManager::GuiManager() {
    static const GLfloat g_vertex_buffer_data[] = {
        -1.0f, 1.0f, 0.0f,
        -1.0f, 0.75f, 0.0f,
        -0.5f, 0.75f, 0.0f,
        -0.5f, 1.0f, 0.0f
    };

    static const GLfloat tex_buffer_data[] = {
        0.0f, 0.0f, 
        0.0f, 1.0f, 
        1.0f, 1.0f, 
        1.0f, 0.0f
    };

    guiShader = make_shared<Program>();
    guiShader->setShaderNames("/home/darryl/Documents/ATTRACT/resources/guiVert.glsl", "/home/darryl/Documents/ATTRACT/resources/guiFrag.glsl");
    guiShader->init();
    guiShader->addAttribute("pos");

    guiShader->addAttribute("posTex");
    guiShader->addUniform("guiTex");

    guiTexture = make_shared<Texture>();
    guiTexture->setFilename("/home/darryl/Documents/ATTRACT/resources/menu.jpg");
    guiTexture->init();
    guiTexture->setUnit(0);
    guiTexture->setWrapModes(GL_REPEAT, GL_REPEAT);




    // Generate 1 buffer, put the resulting identifier in vertexbuffer
    glGenBuffers(1, &vertexbuffer);
    glGenBuffers(1, &texbuffer);
    // The following commands will talk about our 'vertexbuffer' buffer
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    // Give our vertices to OpenGL.
    glBufferData(GL_ARRAY_BUFFER, sizeof (g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, texbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof (tex_buffer_data), tex_buffer_data, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);



}

GuiManager::GuiManager(const GuiManager& orig) {
}

GuiManager::~GuiManager() {
}

void GuiManager::draw() {
    glDisable(GL_DEPTH_TEST);

    guiShader->bind();
    guiTexture->bind(guiShader->getUniform("guiTex"));

    int pos = guiShader->getAttribute("pos");
    glEnableVertexAttribArray(pos);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(
            pos, // attribute 0. No particular reason for 0, but must match the layout in the shader.
            3, // size
            GL_FLOAT, // type
            GL_FALSE, // normalized?
            0, // stride
            (void*) 0 // array buffer offset
            );
    
    glEnableVertexAttribArray(guiShader->getAttribute("posTex"));
    glBindBuffer(GL_ARRAY_BUFFER, texbuffer);
    glVertexAttribPointer(guiShader->getAttribute("posTex"),  2, GL_FLOAT, GL_FALSE, 0, (void *) 0);
    
    
    // Draw the triangle !
    glDrawArrays(GL_QUADS, 0, 4); // Starting from vertex 0; 3 vertices total -> 1 triangle
    glDisableVertexAttribArray(pos);


    guiShader->unbind();
    glEnable(GL_DEPTH_TEST);

}

