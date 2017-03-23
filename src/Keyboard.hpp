//
//  Keyboard.hpp
//  Lab1
//
//  Created by Cody Rhoads on 1/19/17.
//
//

#ifndef Keyboard_hpp
#define Keyboard_hpp

#include <GLFW/glfw3.h>

class Keyboard
{
public:
    static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
    static bool isPressed(unsigned int key);
    static bool setKey(unsigned int key, bool b) {keyToggles[key] = b;}
private:
    static bool keyToggles[349];
};

#endif /* Keyboard_hpp */
