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

// This function is called when the mouse moves
void Mouse::cursor_position_callback(GLFWwindow* window, double xmouse, double ymouse)
{
    mouseX = xmouse;
    mouseY = ymouse;
}
