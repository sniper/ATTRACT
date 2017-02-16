//
//  Keyboard.cpp
//  Lab1
//
//  Created by Cody Rhoads on 1/19/17.
//
//

#include "Keyboard.hpp"

bool Keyboard::keyToggles[349] = {false};
bool Keyboard::lastKeyToggles[349] = {false};
bool Keyboard::hideCursor;

// This function is called when a key is pressed
void Keyboard::key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    if (key == GLFW_KEY_M && action == GLFW_PRESS) {
        hideCursor = !hideCursor;
        if (hideCursor) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        } else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    }

    lastKeyToggles[key] = keyToggles[key];
    if (action == GLFW_PRESS) {
        keyToggles[key] = true;
    } else if (action == GLFW_RELEASE) {
        keyToggles[key] = false;
    }
    //keyToggles[key] = action == GLFW_PRESS;
}

bool Keyboard::isPressed(unsigned int key)
{
    if (key == GLFW_KEY_P || key == GLFW_KEY_J || key == GLFW_KEY_K || key == GLFW_KEY_ENTER ||
            key == GLFW_KEY_1 || key == GLFW_KEY_2 || key == GLFW_KEY_3 || key == GLFW_KEY_4 || key == GLFW_KEY_DELETE) {
        if (!lastKeyToggles[key] && keyToggles[key]) {
            lastKeyToggles[key] = true;
            return true;
        } else {
            return false;
        }
    } else {
        return keyToggles[key];
    }
}
