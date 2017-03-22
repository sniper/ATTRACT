/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   GuiManager.hpp
 * Author: root
 *
 * Created on February 9, 2017, 3:36 PM
 */

#ifndef GUIMANAGER_HPP
#define GUIMANAGER_HPP


#include <GL/glew.h>
#define GLEW_STATIC

#include <GLFW/glfw3.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Program.h"
#include "Texture.h"
#include "MatrixStack.h"


#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>
#include "GameManager.hpp"

class Texture;



class GuiManager {
public:
    GuiManager(std::string resource);
    GuiManager(const GuiManager& orig);
    virtual ~GuiManager();
    void drawAll();
    void drawMenu();
    void resetMenu();
    void update();
    void drawBlack(float alpha);
    void drawEnd(float alpha);
    void drawCutscene(glm::mat4 V);
    void drawSkip(int time);
    void drawPause(int level);
    void drawDeath();
    void drawWin(int level);
    void drawHUD(bool lookingAtMagnet, bool leftClick, bool rightClick);
    void setWindowSize(float w, float h) {width = w; height = h;}
    State interpretMenuPressedKeys(std::vector<char> pressedKeys);
    State interpretPausePressedKeys(std::vector<char> pressedKeys);
    State interpretDeathPressedKeys(std::vector<char> pressedKeys);
    State interpretWinPressedKeys(std::vector<char> pressedKeys);
    
    void resetDeath();
    void resetWin();
private:
    std::string RESOURCE_DIR;
    std::shared_ptr<Program> guiShader;

    std::map<std::string,std::shared_ptr<Texture>> guiTextures;
    std::map<std::string, glm::vec3> scales;
    std::map<std::string, glm::vec3> translates;
    void addTexture(std::string name, glm::vec3 scale, glm::vec3 trans);

    std::string selectedName;

    
    
    GLuint vertexbuffer;
    GLuint texbuffer;
    GLuint vao;

    void draw(std::string name);
    void draw(std::string name, glm::mat4 V);
    void draw(std::string name, float alpha);

    int width, height;
};

#endif /* GUIMANAGER_HPP */

