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
#include "GameObject.hpp"
#include "Mouse.hpp"
#include "Keyboard.hpp"
#include "FmodManager.hpp"

using namespace std;

InputManager::InputManager(shared_ptr<Camera> &camera) :
camera(camera) {

}

InputManager::~InputManager() {

}

State InputManager::processGameInputs(shared_ptr<BulletManager> &bullet,
        shared_ptr<FmodManager> &fmod, shared_ptr<GameObject> obj) {
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
    if (Keyboard::isPressed(GLFW_KEY_ESCAPE)) {
        if (!fmod->isPlaying("select")) {
            fmod->playSound("select", false, 1);
        }
        return PAUSE;
    }


    //my stuff
    if (Keyboard::isPressed(GLFW_KEY_U)) {
        obj->setDirection(obj->getDirection() + vec3(0.01, 0, 0));
    }
    if (Keyboard::isPressed(GLFW_KEY_I)) {
        obj->setDirection(obj->getDirection() + vec3(-0.01, 0, 0));
    }
    if (Keyboard::isPressed(GLFW_KEY_J)) {
        //obj->setDirection(obj->getDirection()+vec3(0,0.01,0));
        obj->setYRot(obj->getYRot() + 0.01);
    }
    if (Keyboard::isPressed(GLFW_KEY_K)) {
        //obj->setDirection(obj->getDirection() + vec3(0, -0.01, 0));
        obj->setYRot(obj->getYRot() - 0.01);
    }
    if (Keyboard::isPressed(GLFW_KEY_N)) {
        obj->setDirection(obj->getDirection() + vec3(0, 0, 0.001));
    }
    if (Keyboard::isPressed(GLFW_KEY_M)) {
        obj->setDirection(obj->getDirection() + vec3(0, 0, -0.001));
    }
    
    
    if (Keyboard::isPressed(GLFW_KEY_T)) {
        obj->setPosition(obj->getPosition() + vec3(0.01, 0, 0));
    }
    if (Keyboard::isPressed(GLFW_KEY_Y)) {
        obj->setPosition(obj->getPosition() + vec3(-0.01, 0, 0));
    }
    if (Keyboard::isPressed(GLFW_KEY_G)) {
        obj->setPosition(obj->getPosition()+vec3(0,0.01,0));
    }
    if (Keyboard::isPressed(GLFW_KEY_H)) {
        obj->setPosition(obj->getPosition() + vec3(0, -0.01, 0));
    }
    if (Keyboard::isPressed(GLFW_KEY_V)) {
        obj->setPosition(obj->getPosition() + vec3(0, 0, 0.01));
    }
    if (Keyboard::isPressed(GLFW_KEY_B)) {
        obj->setPosition(obj->getPosition() + vec3(0, 0, -0.01));
    }
    printf("dir: %f %f %f yrot: %f\n", obj->getDirection().x, obj->getDirection().y, obj->getDirection().z, obj->getYRot());
    printf("pos: %f %f %f\n", obj->getPosition().x, obj->getPosition().y, obj->getPosition().z);
    camera->interpretPressedKeys(pressedKeys, bullet, fmod);
    return GAME;
}

State InputManager::processMenuInputs(shared_ptr<GuiManager> &gui,
        shared_ptr<FmodManager> &fmod) {

    vector<char> pressedKeys;

    if (Keyboard::isPressed(GLFW_KEY_W) || Keyboard::isPressed(GLFW_KEY_UP)) {
        pressedKeys.push_back('w');
        if (!fmod->isPlaying("select")) {
            fmod->playSound("select", false, 1);
        }

    }
    if (Keyboard::isPressed(GLFW_KEY_S) || Keyboard::isPressed(GLFW_KEY_DOWN)) {
        pressedKeys.push_back('s');
        if (!fmod->isPlaying("select")) {
            fmod->playSound("select", false, 1);
        }
    }

    if (Keyboard::isPressed(GLFW_KEY_ENTER)) {
        pressedKeys.push_back('\n');
        if (!fmod->isPlaying("choose")) {
            fmod->playSound("choose", false, 1);
        }
    }
    return gui->interpretMenuPressedKeys(pressedKeys);

}

State InputManager::processPauseInputs(shared_ptr<GuiManager> &gui,
        shared_ptr<FmodManager> &fmod) {
    vector<char> pressedKeys;
    if (!Keyboard::isPressed(GLFW_KEY_ESCAPE)) {
        pressedKeys.push_back('<');
        if (!fmod->isPlaying("choose")) {
            fmod->playSound("choose", false, 1);
        }
    }
    if (Keyboard::isPressed(GLFW_KEY_ENTER)) {
        pressedKeys.push_back('\n');
        if (!fmod->isPlaying("choose")) {
            fmod->playSound("choose", false, 1);
        }
    }
    return gui->interpretPausePressedKeys(pressedKeys);
}

State InputManager::processDeathInputs(shared_ptr<GuiManager> &gui,
        shared_ptr<FmodManager> &fmod) {

    vector<char> pressedKeys;

    if (Keyboard::isPressed(GLFW_KEY_W) || Keyboard::isPressed(GLFW_KEY_UP)) {
        pressedKeys.push_back('w');
        if (!fmod->isPlaying("select")) {
            fmod->playSound("select", false, 1);
        }


    }
    if (Keyboard::isPressed(GLFW_KEY_S) || Keyboard::isPressed(GLFW_KEY_DOWN)) {
        pressedKeys.push_back('s');
        if (!fmod->isPlaying("select")) {
            fmod->playSound("select", false, 1);
        }
    }

    if (Keyboard::isPressed(GLFW_KEY_ENTER)) {
        pressedKeys.push_back('\n');
        if (!fmod->isPlaying("choose")) {
            fmod->playSound("choose", false, 1);
        }
    }
    return gui->interpretDeathPressedKeys(pressedKeys);

}

State InputManager::processWinInputs(shared_ptr<GuiManager> &gui,
        shared_ptr<FmodManager> &fmod) {

    vector<char> pressedKeys;

    if (Keyboard::isPressed(GLFW_KEY_W) || Keyboard::isPressed(GLFW_KEY_UP)) {
        pressedKeys.push_back('w');
        if (!fmod->isPlaying("select")) {
            fmod->playSound("select", false, 1);
        }

    }
    if (Keyboard::isPressed(GLFW_KEY_S) || Keyboard::isPressed(GLFW_KEY_DOWN)) {
        pressedKeys.push_back('s');
        if (!fmod->isPlaying("select")) {
            fmod->playSound("select", false, 1);
        }
    }

    if (Keyboard::isPressed(GLFW_KEY_ENTER)) {
        pressedKeys.push_back('\n');
        if (!fmod->isPlaying("choose")) {
            fmod->playSound("choose", false, 1);
        }
    }
    return gui->interpretWinPressedKeys(pressedKeys);

}
