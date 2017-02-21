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

GuiManager::GuiManager(string resource) :
selectedName("play"),
RESOURCE_DIR(resource) {
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
    guiShader->setShaderNames(RESOURCE_DIR + "guiVert.glsl", RESOURCE_DIR + "guiFrag.glsl");
    guiShader->init();
    guiShader->addAttribute("pos");

    guiShader->addAttribute("posTex");
    guiShader->addUniform("guiTex");
    guiShader->addUniform("M");



    shared_ptr<Texture> playTex = make_shared<Texture>();
    playTex->setFilename(RESOURCE_DIR + "textures/menu/play_select.jpg");
    playTex->init();
    playTex->setUnit(0);
    playTex->setWrapModes(GL_REPEAT, GL_REPEAT);
    guiTextures.insert(make_pair("play_select", playTex));
    scales.insert(make_pair("play_select", vec3(1, 0.7, 1)));
    translates.insert(make_pair("play_select", vec3(0, -0.1, 0)));

    playTex = make_shared<Texture>();
    playTex->setFilename(RESOURCE_DIR + "textures/menu/play_noselect.jpg");
    playTex->init();
    playTex->setUnit(0);
    playTex->setWrapModes(GL_REPEAT, GL_REPEAT);
    guiTextures.insert(make_pair("play_noselect", playTex));
    scales.insert(make_pair("play_noselect", vec3(1, 0.7, 1)));
    translates.insert(make_pair("play_noselect", vec3(0, -0.1, 0)));

    shared_ptr<Texture> playTex1 = make_shared<Texture>();
    playTex1->setFilename(RESOURCE_DIR + "textures/menu/attract.jpg");
    playTex1->init();
    playTex1->setUnit(0);
    playTex1->setWrapModes(GL_REPEAT, GL_REPEAT);
    guiTextures.insert(make_pair("attract", playTex1));
    scales.insert(make_pair("attract", vec3(1, 0.55, 1)));
    translates.insert(make_pair("attract", vec3(0, 0.5, 0)));

    shared_ptr<Texture> playTex2 = make_shared<Texture>();
    playTex2->setFilename(RESOURCE_DIR + "quit.png");
    playTex2->init();
    playTex2->setUnit(0);
    playTex2->setWrapModes(GL_REPEAT, GL_REPEAT);
    guiTextures.insert(make_pair("quit", playTex2));
    scales.insert(make_pair("quit", vec3(1, 0.35, 1)));
    translates.insert(make_pair("quit", vec3(0, -0.8, 0)));

    playTex2 = make_shared<Texture>();
    playTex2->setFilename(RESOURCE_DIR + "quit_select.png");
    playTex2->init();
    playTex2->setUnit(0);
    playTex2->setWrapModes(GL_REPEAT, GL_REPEAT);
    guiTextures.insert(make_pair("quit_select", playTex2));
    scales.insert(make_pair("quit_select", vec3(1, 0.7, 1)));
    translates.insert(make_pair("quit_select", vec3(0, -0.6, 0)));

    playTex2 = make_shared<Texture>();
    playTex2->setFilename(RESOURCE_DIR + "quit_noselect.png");
    playTex2->init();
    playTex2->setUnit(0);
    playTex2->setWrapModes(GL_REPEAT, GL_REPEAT);
    guiTextures.insert(make_pair("quit_noselect", playTex2));
    scales.insert(make_pair("quit_noselect", vec3(1, 0.7, 1)));
    translates.insert(make_pair("quit_noselect", vec3(0, -0.6, 0)));

    shared_ptr<Texture> playTex3 = make_shared<Texture>();
    playTex3->setFilename(RESOURCE_DIR + "arrow.jpg");
    playTex3->init();
    playTex3->setUnit(0);
    playTex3->setWrapModes(GL_REPEAT, GL_REPEAT);
    guiTextures.insert(make_pair("arrow", playTex3));
    scales.insert(make_pair("arrow", vec3(0.3, 0.2, 1)));
    translates.insert(make_pair("arrow", vec3(-0.5, -0.2, 0)));

    shared_ptr<Texture> playTex4 = make_shared<Texture>();
    playTex4->setFilename(RESOURCE_DIR + "pause.png");
    playTex4->init();
    playTex4->setUnit(0);
    playTex4->setWrapModes(GL_REPEAT, GL_REPEAT);
    guiTextures.insert(make_pair("pause", playTex4));
    scales.insert(make_pair("pause", vec3(1, 1, 1)));
    translates.insert(make_pair("pause", vec3(-0.5, 0.5, 0)));

    shared_ptr<Texture> playTex5 = make_shared<Texture>();
    playTex5->setFilename(RESOURCE_DIR + "shipparts.png");
    playTex5->init();
    playTex5->setUnit(0);
    playTex5->setWrapModes(GL_REPEAT, GL_REPEAT);
    guiTextures.insert(make_pair("shipparts", playTex5));
    scales.insert(make_pair("shipparts", vec3(0.7, 0.7, 1)));
    translates.insert(make_pair("shipparts", vec3(0.5, 0.5, 0)));

    playTex5 = make_shared<Texture>();
    playTex5->setFilename(RESOURCE_DIR + "shipparts1.png");
    playTex5->init();
    playTex5->setUnit(0);
    playTex5->setWrapModes(GL_REPEAT, GL_REPEAT);
    guiTextures.insert(make_pair("shipparts1", playTex5));
    scales.insert(make_pair("shipparts1", vec3(1.5, 1.5, 1)));
    translates.insert(make_pair("shipparts1", vec3(0.5, 0.2, 0)));

    playTex5 = make_shared<Texture>();
    playTex5->setFilename(RESOURCE_DIR + "death.png");
    playTex5->init();
    playTex5->setUnit(0);
    playTex5->setWrapModes(GL_REPEAT, GL_REPEAT);
    guiTextures.insert(make_pair("death", playTex5));
    scales.insert(make_pair("death", vec3(0.7, 0.7, 1)));
    translates.insert(make_pair("death", vec3(0, 0.5, 0)));

    playTex5 = make_shared<Texture>();
    playTex5->setFilename(RESOURCE_DIR + "win.jpg");
    playTex5->init();
    playTex5->setUnit(0);
    playTex5->setWrapModes(GL_REPEAT, GL_REPEAT);
    guiTextures.insert(make_pair("win", playTex5));
    scales.insert(make_pair("win", vec3(0.7, 0.7, 1)));
    translates.insert(make_pair("win", vec3(-0.4, 0.5, 0)));

    playTex5 = make_shared<Texture>();
    playTex5->setFilename(RESOURCE_DIR + "nextlevel.jpg");
    playTex5->init();
    playTex5->setUnit(0);
    playTex5->setWrapModes(GL_REPEAT, GL_REPEAT);
    guiTextures.insert(make_pair("nextlevel", playTex5));
    scales.insert(make_pair("nextlevel", vec3(0.7, 0.7, 1)));
    translates.insert(make_pair("nextlevel", vec3(0, -0.25, 0)));

    playTex5 = make_shared<Texture>();
    playTex5->setFilename(RESOURCE_DIR + "tryagain_noselect.png");
    playTex5->init();
    playTex5->setUnit(0);
    playTex5->setWrapModes(GL_REPEAT, GL_REPEAT);
    guiTextures.insert(make_pair("tryagain_noselect", playTex5));
    scales.insert(make_pair("tryagain_noselect", vec3(0.7, 0.7, 1)));
    translates.insert(make_pair("tryagain_noselect", vec3(0, -0.18, 0)));

    playTex5 = make_shared<Texture>();
    playTex5->setFilename(RESOURCE_DIR + "tryagain_select.png");
    playTex5->init();
    playTex5->setUnit(0);
    playTex5->setWrapModes(GL_REPEAT, GL_REPEAT);
    guiTextures.insert(make_pair("tryagain_select", playTex5));
    scales.insert(make_pair("tryagain_select", vec3(0.7, 0.7, 1)));
    translates.insert(make_pair("tryagain_select", vec3(0, -0.18, 0)));

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

State GuiManager::interpretMenuPressedKeys(vector<char> pressedKeys) {
    if (find(pressedKeys.begin(), pressedKeys.end(), 's') != pressedKeys.end()) {
        translates["arrow"] = vec3(-0.8, -0.8, 0);
        selectedName = "quit";
    } else if (find(pressedKeys.begin(), pressedKeys.end(), 'w') != pressedKeys.end()) {
        translates["arrow"] = vec3(-0.5, -0.2, 0);
        selectedName = "play";
    } else if (find(pressedKeys.begin(), pressedKeys.end(), '\n') != pressedKeys.end()) {
        if (selectedName == "play")
            return GAME;
        else if (selectedName == "quit")
            exit(0);
    }
    return MENU;
}

State GuiManager::interpretPausePressedKeys(std::vector<char> pressedKeys) {
    if (find(pressedKeys.begin(), pressedKeys.end(), '<') != pressedKeys.end()) {
        return GAME;
    } else if (find(pressedKeys.begin(), pressedKeys.end(), '\n') != pressedKeys.end()) {
        if (selectedName == "quit")
            exit(0);
    }

    return PAUSE;
}

State GuiManager::interpretDeathPressedKeys(std::vector<char> pressedKeys) {
    if (find(pressedKeys.begin(), pressedKeys.end(), 's') != pressedKeys.end()) {

        selectedName = "quit";
    } else if (find(pressedKeys.begin(), pressedKeys.end(), 'w') != pressedKeys.end()) {

        selectedName = "tryagain";
    } else if (find(pressedKeys.begin(), pressedKeys.end(), '\n') != pressedKeys.end()) {
        if (selectedName == "quit")
            exit(0);
        else if (selectedName == "tryagain")
            return GAME;
    }
    return DEATH;
}

State GuiManager::interpretWinPressedKeys(std::vector<char> pressedKeys) {
    if (find(pressedKeys.begin(), pressedKeys.end(), 's') != pressedKeys.end()) {
        translates["arrow"] = vec3(-0.8, -0.8, 0);
        selectedName = "quit";
    } else if (find(pressedKeys.begin(), pressedKeys.end(), 'w') != pressedKeys.end()) {
        translates["arrow"] = vec3(-0.5, -0.2, 0);
        selectedName = "nextlevel";
    } else if (find(pressedKeys.begin(), pressedKeys.end(), '\n') != pressedKeys.end()) {
        if (selectedName == "quit")
            exit(0);
        else if (selectedName == "nextlevel")
            return GAME;
    }
    return WIN;
}

void GuiManager::drawMenu() {
    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    draw("attract");
    if (selectedName == "play") {
        draw("play_select");
        draw("quit_noselect");
    } else {
        draw("play_noselect");
        draw("quit_select");
    }

    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);



}

void GuiManager::drawPause(int level) {


    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    draw("pause");
    draw("quit_select");
    draw("shipparts");


    selectedName = "quit";
    switch (level) {
        case 1:
            break;

        case 2:
            draw("shipparts1");
            break;
    }

    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);
}

void GuiManager::drawDeath() {
    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    draw("death");
    
    if (selectedName == "quit") {
        draw("quit_select");
        draw("tryagain_noselect");
    } else {
        draw("quit_noselect");
        draw("tryagain_select");
    }



    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);
}

void GuiManager::drawWin(int level) {
    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    draw("win");
    draw("nextlevel");
    
    if(selectedName == "quit") {
        draw("quit_select");
    }
    else {
        draw("quit_noselect");
    }
    
    draw("shipparts");
    switch(level) {
        case 1:
            draw("shipparts1");
            break;
    }
    
    
    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);
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

