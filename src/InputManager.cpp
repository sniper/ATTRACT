//
//  InputManager.cpp
//  Lab1
//
//  Created by Cody Rhoads on 1/18/17.
//
//

#include <vector>
#include <iostream>

#include "InputManager.hpp"
#include "Camera.hpp"
#include "Mouse.hpp"
#include "Keyboard.hpp"

using namespace std;

InputManager::InputManager(shared_ptr<Camera> &camera) :
camera(camera)
{

}

InputManager::~InputManager()
{

}

void InputManager::processGameInputs(BulletManager *bullet)
{
    // Move mouse.
    camera->mouseMoved(Mouse::getMouseX(), Mouse::getMouseY());

    // Create a vector to hold the keyboard inputs
    vector<char> pressedKeys;

    if (Keyboard::isPressed(GLFW_KEY_W) || Keyboard::isPressed(GLFW_KEY_UP)) {
        pressedKeys.push_back('w');
    }
    if (Keyboard::isPressed(GLFW_KEY_S) || Keyboard::isPressed(GLFW_KEY_DOWN)) {
        pressedKeys.push_back('s');
    }
    if (Keyboard::isPressed(GLFW_KEY_A) || Keyboard::isPressed(GLFW_KEY_LEFT)) {
        pressedKeys.push_back('a');
    }
    if (Keyboard::isPressed(GLFW_KEY_D) || Keyboard::isPressed(GLFW_KEY_RIGHT)) {
        pressedKeys.push_back('d');
    }
    if (Keyboard::isPressed(GLFW_KEY_SPACE)) {
        pressedKeys.push_back(' ');
    }
    if (Keyboard::isPressed(GLFW_KEY_P)) {
        pressedKeys.push_back('p');
    }
    if (Keyboard::isPressed(GLFW_KEY_I)) {
        pressedKeys.push_back('i');
    }
    if (Keyboard::isPressed(GLFW_KEY_O)) {
        pressedKeys.push_back('o');
    }

    camera->interpretPressedKeys(pressedKeys, bullet);
}
