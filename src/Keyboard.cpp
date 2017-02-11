//
//  Keyboard.cpp
//  Lab1
//
//  Created by Cody Rhoads on 1/19/17.
//
//

#include "Keyboard.hpp"

#include <iostream>
bool Keyboard::keyToggles[349] = {false};
static bool hideCursor = true;



// This function is called when a key is pressed

void Keyboard::key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {


    if (key == GLFW_KEY_M && action == GLFW_PRESS) {
        hideCursor = !hideCursor;
        if (hideCursor) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        } else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    }

    if (key == GLFW_KEY_ESCAPE) {
        if (action == GLFW_RELEASE) {
            keyToggles[key] = !keyToggles[key];
        }


    } else if (action == GLFW_PRESS) {
        keyToggles[key] = true;
    } else if (action == GLFW_RELEASE) {
        keyToggles[key] = false;
    }




}

bool Keyboard::isPressed(unsigned int key) {
    return keyToggles[key];
}
