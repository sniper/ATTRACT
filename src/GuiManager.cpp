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
#include "GLSL.h"

using namespace std;
using namespace glm;

GuiManager::GuiManager(string resource) :
selectedName("play"),
RESOURCE_DIR(resource) {
    static const GLfloat g_vertex_buffer_data[] = {
        -0.5f, 0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.5f, 0.5f, 0.0f,
        -0.5f, 0.5f, 0.0f,
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
    guiShader->addUniform("P");

    GLSL::checkError(GET_FILE_LINE);

    shared_ptr<Texture> playTex = make_shared<Texture>();

    playTex->setFilename(RESOURCE_DIR + "textures/play_select.png");

    playTex->init();


    playTex->setUnit(0);


    playTex->setWrapModes(GL_REPEAT, GL_REPEAT);


    guiTextures.insert(make_pair("play_select", playTex));
    scales.insert(make_pair("play_select", vec3(1, 0.7, 1)));
    translates.insert(make_pair("play_select", vec3(0, -0.1, 0)));

        playTex = make_shared<Texture>();
        playTex->setFilename(RESOURCE_DIR + "textures/play_noselect.png");
        playTex->init();
        playTex->setUnit(0);
        playTex->setWrapModes(GL_REPEAT, GL_REPEAT);
        guiTextures.insert(make_pair("play_noselect", playTex));
        scales.insert(make_pair("play_noselect", vec3(1, 0.7, 1)));
        translates.insert(make_pair("play_noselect", vec3(0, -0.1, 0)));
     

    shared_ptr<Texture> playTex1 = make_shared<Texture>();
    playTex1->setFilename(RESOURCE_DIR + "textures/attract.png");
    playTex1->init();
    playTex1->setUnit(0);
    playTex1->setWrapModes(GL_REPEAT, GL_REPEAT);
    guiTextures.insert(make_pair("attract", playTex1));
    scales.insert(make_pair("attract", vec3(1, 0.55, 1)));
    translates.insert(make_pair("attract", vec3(0, 0.5, 0)));
    
     shared_ptr<Texture> playTex2 = make_shared<Texture>();
     playTex2->setFilename(RESOURCE_DIR + "textures/quit.png");
     playTex2->init();
     playTex2->setUnit(0);
     playTex2->setWrapModes(GL_REPEAT, GL_REPEAT);
     guiTextures.insert(make_pair("quit", playTex2));
     scales.insert(make_pair("quit", vec3(1, 0.35, 1)));
     translates.insert(make_pair("quit", vec3(0, -0.8, 0)));

     playTex2 = make_shared<Texture>();
     playTex2->setFilename(RESOURCE_DIR + "textures/quit_select.png");
     playTex2->init();
     playTex2->setUnit(0);
     playTex2->setWrapModes(GL_REPEAT, GL_REPEAT);
     guiTextures.insert(make_pair("quit_select", playTex2));
     scales.insert(make_pair("quit_select", vec3(1, 0.7, 1)));
     translates.insert(make_pair("quit_select", vec3(0, -0.6, 0)));

     playTex2 = make_shared<Texture>();
     playTex2->setFilename(RESOURCE_DIR + "textures/quit_noselect.png");
     playTex2->init();
     playTex2->setUnit(0);
     playTex2->setWrapModes(GL_REPEAT, GL_REPEAT);
     guiTextures.insert(make_pair("quit_noselect", playTex2));
     scales.insert(make_pair("quit_noselect", vec3(1, 0.7, 1)));
     translates.insert(make_pair("quit_noselect", vec3(0, -0.6, 0)));

     shared_ptr<Texture> playTex4 = make_shared<Texture>();
     playTex4->setFilename(RESOURCE_DIR + "textures/pause.png");
     playTex4->init();
     playTex4->setUnit(0);
     playTex4->setWrapModes(GL_REPEAT, GL_REPEAT);
     guiTextures.insert(make_pair("pause", playTex4));
     scales.insert(make_pair("pause", vec3(1, 1, 1)));
     translates.insert(make_pair("pause", vec3(-0.5, 0.5, 0)));

     shared_ptr<Texture> playTex5 = make_shared<Texture>();
     playTex5->setFilename(RESOURCE_DIR + "textures/shipparts.png");
     playTex5->init();
     playTex5->setUnit(0);
     playTex5->setWrapModes(GL_REPEAT, GL_REPEAT);
     guiTextures.insert(make_pair("shipparts", playTex5));
     scales.insert(make_pair("shipparts", vec3(0.7, 0.7, 1)));
     translates.insert(make_pair("shipparts", vec3(0.5, 0.5, 0)));

     playTex5 = make_shared<Texture>();
     playTex5->setFilename(RESOURCE_DIR + "textures/shipparts1.png");
     playTex5->init();
     playTex5->setUnit(0);
     playTex5->setWrapModes(GL_REPEAT, GL_REPEAT);
     guiTextures.insert(make_pair("shipparts1", playTex5));
     scales.insert(make_pair("shipparts1", vec3(1.5, 1.5, 1)));
     translates.insert(make_pair("shipparts1", vec3(0.5, 0.2, 0)));

     playTex5 = make_shared<Texture>();
     playTex5->setFilename(RESOURCE_DIR + "textures/death.png");
     playTex5->init();
     playTex5->setUnit(0);
     playTex5->setWrapModes(GL_REPEAT, GL_REPEAT);
     guiTextures.insert(make_pair("death", playTex5));
     scales.insert(make_pair("death", vec3(0.7, 0.7, 1)));
     translates.insert(make_pair("death", vec3(0, 0.5, 0)));

     playTex5 = make_shared<Texture>();
     playTex5->setFilename(RESOURCE_DIR + "textures/win.png");
     playTex5->init();
     playTex5->setUnit(0);
     playTex5->setWrapModes(GL_REPEAT, GL_REPEAT);
     guiTextures.insert(make_pair("win", playTex5));
     scales.insert(make_pair("win", vec3(0.7, 0.7, 1)));
     translates.insert(make_pair("win", vec3(-0.4, 0.5, 0)));

     playTex5 = make_shared<Texture>();
     playTex5->setFilename(RESOURCE_DIR + "textures/nextlevel_select.png");
     playTex5->init();
     playTex5->setUnit(0);
     playTex5->setWrapModes(GL_REPEAT, GL_REPEAT);
     guiTextures.insert(make_pair("nextlevel_select", playTex5));
     scales.insert(make_pair("nextlevel_select", vec3(0.7, 0.7, 1)));
     translates.insert(make_pair("nextlevel_select", vec3(0, -0.25, 0)));

     playTex5 = make_shared<Texture>();
     playTex5->setFilename(RESOURCE_DIR + "textures/nextlevel_noselect.png");
     playTex5->init();
     playTex5->setUnit(0);
     playTex5->setWrapModes(GL_REPEAT, GL_REPEAT);
     guiTextures.insert(make_pair("nextlevel_noselect", playTex5));
     scales.insert(make_pair("nextlevel_noselect", vec3(0.7, 0.7, 1)));
     translates.insert(make_pair("nextlevel_noselect", vec3(0, -0.25, 0)));

     playTex5 = make_shared<Texture>();
     playTex5->setFilename(RESOURCE_DIR + "textures/tryagain_noselect.png");
     playTex5->init();
     playTex5->setUnit(0);
     playTex5->setWrapModes(GL_REPEAT, GL_REPEAT);
     guiTextures.insert(make_pair("tryagain_noselect", playTex5));
     scales.insert(make_pair("tryagain_noselect", vec3(0.7, 0.7, 1)));
     translates.insert(make_pair("tryagain_noselect", vec3(0, -0.18, 0)));

     playTex5 = make_shared<Texture>();
     playTex5->setFilename(RESOURCE_DIR + "textures/tryagain_select.png");
     playTex5->init();
     playTex5->setUnit(0);
     playTex5->setWrapModes(GL_REPEAT, GL_REPEAT);
     guiTextures.insert(make_pair("tryagain_select", playTex5));
     scales.insert(make_pair("tryagain_select", vec3(0.7, 0.7, 1)));
     translates.insert(make_pair("tryagain_select", vec3(0, -0.18, 0)));
     
     playTex5 = make_shared<Texture>();
     playTex5->setFilename(RESOURCE_DIR + "textures/reticle_center_off.png");
     playTex5->init();
     playTex5->setUnit(0);
     playTex5->setWrapModes(GL_REPEAT, GL_REPEAT);
     guiTextures.insert(make_pair("reticle_center_off", playTex5));
     scales.insert(make_pair("reticle_center_off", vec3(0.25, 0.25, 1)));
     translates.insert(make_pair("reticle_center_off", vec3(0, 0, 0)));

     playTex5 = make_shared<Texture>();
     playTex5->setFilename(RESOURCE_DIR + "textures/reticle_center_on.png");
     playTex5->init();
     playTex5->setUnit(0);
     playTex5->setWrapModes(GL_REPEAT, GL_REPEAT);
     guiTextures.insert(make_pair("reticle_center_on", playTex5));
     scales.insert(make_pair("reticle_center_on", vec3(0.25, 0.25, 1)));
     translates.insert(make_pair("reticle_center_on", vec3(0, 0, 0)));

     playTex5 = make_shared<Texture>();
     playTex5->setFilename(RESOURCE_DIR + "textures/reticle_top_off.png");
     playTex5->init();
     playTex5->setUnit(0);
     playTex5->setWrapModes(GL_REPEAT, GL_REPEAT);
     guiTextures.insert(make_pair("reticle_top_off", playTex5));
     scales.insert(make_pair("reticle_top_off", vec3(0.25, 0.25, 1)));
     translates.insert(make_pair("reticle_top_off", vec3(0, 0, 0)));

     playTex5 = make_shared<Texture>();
     playTex5->setFilename(RESOURCE_DIR + "textures/reticle_top_on.png");
     playTex5->init();
     playTex5->setUnit(0);
     playTex5->setWrapModes(GL_REPEAT, GL_REPEAT);
     guiTextures.insert(make_pair("reticle_top_on", playTex5));
     scales.insert(make_pair("reticle_top_on", vec3(0.25, 0.25, 1)));
     translates.insert(make_pair("reticle_top_on", vec3(0, 0, 0)));

     playTex5 = make_shared<Texture>();
     playTex5->setFilename(RESOURCE_DIR + "textures/reticle_bottom_off.png");
     playTex5->init();
     playTex5->setUnit(0);
     playTex5->setWrapModes(GL_REPEAT, GL_REPEAT);
     guiTextures.insert(make_pair("reticle_bottom_off", playTex5));
     scales.insert(make_pair("reticle_bottom_off", vec3(0.25, 0.25, 1)));
     translates.insert(make_pair("reticle_bottom_off", vec3(0, 0, 0)));

     playTex5 = make_shared<Texture>();
     playTex5->setFilename(RESOURCE_DIR + "textures/reticle_bottom_on.png");
     playTex5->init();
     playTex5->setUnit(0);
     playTex5->setWrapModes(GL_REPEAT, GL_REPEAT);
     guiTextures.insert(make_pair("reticle_bottom_on", playTex5));
     scales.insert(make_pair("reticle_bottom_on", vec3(0.25, 0.25, 1)));
     translates.insert(make_pair("reticle_bottom_on", vec3(0, 0, 0)));

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    GLSL::checkError(GET_FILE_LINE);
    // Generate 1 buffer, put the resulting identifier in vertexbuffer
    glGenBuffers(1, &vertexbuffer);
    glGenBuffers(1, &texbuffer);
    GLSL::checkError(GET_FILE_LINE);
    // The following commands will talk about our 'vertexbuffer' buffer
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    // Give our vertices to OpenGL.
    glBufferData(GL_ARRAY_BUFFER, sizeof (g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, texbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof (tex_buffer_data), tex_buffer_data, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    GLSL::checkError(GET_FILE_LINE);
     

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

        selectedName = "quit";
    } else if (find(pressedKeys.begin(), pressedKeys.end(), 'w') != pressedKeys.end()) {

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

void GuiManager::drawHUD(bool lookingAtMagnet, bool leftClick, bool rightClick) {
    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    if (lookingAtMagnet) {
        draw("reticle_center_on");
        if (leftClick) {
            draw("reticle_top_on");
            draw("reticle_bottom_off");
        } else if (rightClick) {
            draw("reticle_top_off");
            draw("reticle_bottom_on");
        } else {
            draw("reticle_top_off");
            draw("reticle_bottom_off");
        }
    } else {
        draw("reticle_center_off");
        draw("reticle_top_off");
        draw("reticle_bottom_off");
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


    if (selectedName == "quit") {
        draw("quit_select");
        draw("nextlevel_noselect");
    } else {
        draw("quit_noselect");
        draw("nextlevel_select");
    }

    draw("shipparts");
    switch (level) {
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

    //auto P = make_shared<MatrixStack>();
    //P->ortho(0.0f, (float) width, 0.0f, (float) height, 0.0f, 10.0f);
    float aspect = (float) width / (float) height;
    mat4 P = glm::perspective(80.0f, aspect,0.0f, 1.0f);

    glDisable(GL_DEPTH_TEST);

    GLSL::checkError(GET_FILE_LINE);

    guiShader->bind();

    //glBindVertexArray(vao);

    guiTextures[name]->bind(guiShader->getUniform("guiTex"));

    GLSL::checkError(GET_FILE_LINE);

    glUniformMatrix4fv(guiShader->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
    glUniformMatrix4fv(guiShader->getUniform("P"), 1, GL_FALSE, value_ptr(P));

    int pos = guiShader->getAttribute("pos");

    glEnableVertexAttribArray(pos);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    GLSL::checkError(GET_FILE_LINE);

    glVertexAttribPointer(
            pos, // attribute 0. No particular reason for 0, but must match the layout in the shader.
            3, // size
            GL_FLOAT, // type
            GL_FALSE, // normalized?
            0, // stride
            (void*) 0 // array buffer offset
            );
    GLSL::checkError(GET_FILE_LINE);

    glEnableVertexAttribArray(guiShader->getAttribute("posTex"));
    glBindBuffer(GL_ARRAY_BUFFER, texbuffer);
    glVertexAttribPointer(guiShader->getAttribute("posTex"), 2, GL_FLOAT, GL_FALSE, 0, (void *) 0);

    GLSL::checkError(GET_FILE_LINE);

    // Draw the triangle !
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 5); // Starting from vertex 0; 3 vertices total -> 1 triangle
   
    GLSL::checkError(GET_FILE_LINE);

    glDisableVertexAttribArray(pos);

    GLSL::checkError(GET_FILE_LINE);

    guiShader->unbind();
    glEnable(GL_DEPTH_TEST);
    M->popMatrix();
}

