//
//  Mouse.cpp
//  Lab1
//
//  Created by Cody Rhoads on 1/19/17.
//
//

#include "Mouse.hpp"

double Mouse::mouseX = 0;
double Mouse::mouseY = 0;
bool Mouse::prevToggles[2] = {false, false};
bool Mouse::mouseToggles[2] = {false, false};

// This function is called when the mouse moves
void Mouse::cursor_position_callback(GLFWwindow* window, double xmouse, double ymouse)
{
    mouseX = xmouse;
    mouseY = ymouse;
}

void Mouse::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    prevToggles[button] = mouseToggles[button];
    mouseToggles[button] = action == GLFW_PRESS;
}

void Mouse::clearClick(int button)
{
    prevToggles[button] = true;
}
