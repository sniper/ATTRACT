//
//  Keyboard.cpp
//  Lab1
//
//  Created by Cody Rhoads on 1/19/17.
//
//

#include "Keyboard.hpp"

bool Keyboard::keyToggles[349] = {false};

// This function is called when a key is pressed
void Keyboard::key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    
    if (action == GLFW_PRESS) {
        keyToggles[key] = true;
    }
    else if (action == GLFW_RELEASE) {
        keyToggles[key] = false;
    }
}

bool Keyboard::isPressed(unsigned int key)
{
    return keyToggles[key];
}
