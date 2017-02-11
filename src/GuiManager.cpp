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
using namespace glm;

GuiManager::GuiManager() {
    static const GLfloat g_vertex_buffer_data[] = {
        -0.5f, 0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.5f, 0.5f, 0.0f
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
    guiShader->addUniform("M");

    shared_ptr<Texture> playTex = make_shared<Texture>();
    playTex->setFilename("/home/darryl/Documents/ATTRACT/resources/play.jpg");
    playTex->init();
    playTex->setUnit(0);
    playTex->setWrapModes(GL_REPEAT, GL_REPEAT);
    guiTextures.insert(make_pair("play", playTex));
    scales.insert(make_pair("play", vec3(0.5, 0.5, 1)));
    translates.insert(make_pair("play", vec3(0, -0.2, 0)));

    shared_ptr<Texture> playTex1 = make_shared<Texture>();
    playTex1->setFilename("/home/darryl/Documents/ATTRACT/resources/attract.jpg");
    playTex1->init();
    playTex1->setUnit(0);
    playTex1->setWrapModes(GL_REPEAT, GL_REPEAT);
    guiTextures.insert(make_pair("attract", playTex1));
    scales.insert(make_pair("attract", vec3(1, 0.55, 1)));
    translates.insert(make_pair("attract", vec3(0, 0.5, 0)));

    shared_ptr<Texture> playTex2 = make_shared<Texture>();
    playTex2->setFilename("/home/darryl/Documents/ATTRACT/resources/quit.jpg");
    playTex2->init();
    playTex2->setUnit(0);
    playTex2->setWrapModes(GL_REPEAT, GL_REPEAT);
    guiTextures.insert(make_pair("quit", playTex2));
    scales.insert(make_pair("quit", vec3(1, 0.35, 1)));
    translates.insert(make_pair("quit", vec3(0, -0.8, 0)));

    shared_ptr<Texture> playTex3 = make_shared<Texture>();
    playTex3->setFilename("/home/darryl/Documents/ATTRACT/resources/arrow.jpg");
    playTex3->init();
    playTex3->setUnit(0);
    playTex3->setWrapModes(GL_REPEAT, GL_REPEAT);
    guiTextures.insert(make_pair("arrow", playTex3));
    scales.insert(make_pair("arrow", vec3(0.3, 0.2, 1)));
    translates.insert(make_pair("arrow", vec3(-0.5, -0.2, 0)));




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

void GuiManager::drawAll() {
    for (std::map<string, shared_ptr < Texture>>::iterator it = guiTextures.begin(); it != guiTextures.end(); ++it) {
        draw(it->first);
    }

}

void GuiManager::draw(string name) {

    auto M = make_shared<MatrixStack>();
    M->pushMatrix();
    M->loadIdentity();

    M->translate(translates[name]);
    M->scale(scales[name]);

    glDisable(GL_DEPTH_TEST);

    guiShader->bind();
    guiTextures[name]->bind(guiShader->getUniform("guiTex"));


    glUniformMatrix4fv(guiShader->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));

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
    glVertexAttribPointer(guiShader->getAttribute("posTex"), 2, GL_FLOAT, GL_FALSE, 0, (void *) 0);


    // Draw the triangle !
    glDrawArrays(GL_QUADS, 0, 4); // Starting from vertex 0; 3 vertices total -> 1 triangle
    glDisableVertexAttribArray(pos);


    guiShader->unbind();
    glEnable(GL_DEPTH_TEST);
    M->popMatrix();
}

