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

void InputManager::processInputs()
{
    // Move mouse.
    camera->mouseMoved(Mouse::getMouseX(), Mouse::getMouseY());
    
    bool leftMouse = Mouse::isLeftMouseButtonPressed();
    bool rightMouse = Mouse::isRightMouseButtonPressed();
    
    // Handle if mouse buttons are pressed. Don't do anything if both are pressed.
    if (leftMouse && !rightMouse) {
        cout << "Left Mouse Button is pressed." << endl;
    }
    else if (rightMouse && !leftMouse) {
        cout << "Right Mouse Button is pressed." << endl;
    }
    
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
    
    camera->interpretPressedKeys(pressedKeys);
}
