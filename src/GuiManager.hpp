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


#include <iostream>
#include <vector>
#include <memory>

class GuiManager {
public:
    GuiManager();
    GuiManager(const GuiManager& orig);
    virtual ~GuiManager();
    void draw();
private:
    std::shared_ptr<Program> guiShader;
    std::shared_ptr<Texture> guiTexture;

    GLuint vertexbuffer;
    GLuint texbuffer;
    

};

#endif /* GUIMANAGER_HPP */

