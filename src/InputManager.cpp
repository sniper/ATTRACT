//
//  InputManager.cpp
//  Lab1
//
//  Created by Cody Rhoads on 1/18/17.
//
//

#include <vector>

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

vector<char> InputManager::processInputs()
{
    // Move mouse.
    camera->mouseMoved(Mouse::getMouseX(), Mouse::getMouseY());

    // Create a vector to hold the keyboard inputs
    vector<char> movementKeys, objectKeys;

    // movement 
    if (Keyboard::isPressed(GLFW_KEY_W)) {
        movementKeys.push_back('w');
    }
    if (Keyboard::isPressed(GLFW_KEY_S)) {
        movementKeys.push_back('s');
    }
    if (Keyboard::isPressed(GLFW_KEY_A)) {
        movementKeys.push_back('a');
    }
    if (Keyboard::isPressed(GLFW_KEY_D)) {
        movementKeys.push_back('d');
    }
    if (Keyboard::isPressed(GLFW_KEY_LEFT_SHIFT)) {
        movementKeys.push_back('^');
        objectKeys.push_back('^');
    }

    // object manipulation
    if (Keyboard::isPressed(GLFW_KEY_P)) {
        objectKeys.push_back('p');
    }
    if (Keyboard::isPressed(GLFW_KEY_J)) {
        objectKeys.push_back('j');
    }
    if (Keyboard::isPressed(GLFW_KEY_K)) {
        objectKeys.push_back('k');
    }
    if (Keyboard::isPressed(GLFW_KEY_X)) {
        objectKeys.push_back('x');
    }
    if (Keyboard::isPressed(GLFW_KEY_Y)) {
        objectKeys.push_back('y');
    }
    if (Keyboard::isPressed(GLFW_KEY_Z)) {
        objectKeys.push_back('z');
    }
    if (Keyboard::isPressed(GLFW_KEY_LEFT)) {
        objectKeys.push_back('L');
    }
    if (Keyboard::isPressed(GLFW_KEY_RIGHT)) {
        objectKeys.push_back('R');
    }
    if (Keyboard::isPressed(GLFW_KEY_UP)) {
        objectKeys.push_back('U');
    }
    if (Keyboard::isPressed(GLFW_KEY_DOWN)) {
        objectKeys.push_back('D');
    }
    if (Keyboard::isPressed(GLFW_KEY_Q)) {
        objectKeys.push_back('q');
    }
    if (Keyboard::isPressed(GLFW_KEY_E)) {
        objectKeys.push_back('e');
    }
    if (Keyboard::isPressed(GLFW_KEY_ENTER)) {
        objectKeys.push_back('E');
    }
    if (Keyboard::isPressed(GLFW_KEY_1)) {
        objectKeys.push_back('1');
    }
    if (Keyboard::isPressed(GLFW_KEY_2)) {
        objectKeys.push_back('2');
    }
    if (Keyboard::isPressed(GLFW_KEY_3)) {
        objectKeys.push_back('3');
    }
    if (Keyboard::isPressed(GLFW_KEY_4)) {
        objectKeys.push_back('4');
    }
    if (Keyboard::isPressed(GLFW_KEY_DELETE)) {
        objectKeys.push_back('X');
    }

    camera->interpretPressedKeys(movementKeys);
    return objectKeys;
}
