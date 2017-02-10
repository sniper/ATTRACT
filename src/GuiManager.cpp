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
    };    
    
    guiShader = make_shared<Program>();
    guiShader->setShaderNames("/home/darryl/Documents/ATTRACT/resources/guiVert.glsl", "/home/darryl/Documents/ATTRACT/resources/guiFrag.glsl");
    guiShader->init();
    guiShader->addAttribute("pos");
    guiShader->addUniform("P");
    
    


    // This will identify our vertex buffer
    cout << "what" <<  endl;    
    // Generate 1 buffer, put the resulting identifier in vertexbuffer
    glGenBuffers(1, &vertexbuffer);
    // The following commands will talk about our 'vertexbuffer' buffer
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    // Give our vertices to OpenGL.
    glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), g_vertex_buffer_data, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
 
    

}

GuiManager::GuiManager(const GuiManager& orig) {
}

GuiManager::~GuiManager() {
}

void GuiManager::draw() {
    glDisable(GL_DEPTH_TEST);
    
    guiShader->bind();
    
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
    // Draw the triangle !
    glDrawArrays(GL_TRIANGLES, 0, 3); // Starting from vertex 0; 3 vertices total -> 1 triangle
    glDisableVertexAttribArray(pos);
    
    
    guiShader->unbind();
    glEnable(GL_DEPTH_TEST);
    
}

