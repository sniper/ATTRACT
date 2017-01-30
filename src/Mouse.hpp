//
//  Mouse.hpp
//  Lab1
//
//  Created by Cody Rhoads on 1/19/17.
//
//

#ifndef Mouse_hpp
#define Mouse_hpp

#include <GLFW/glfw3.h>

class Mouse
{
public:
    static void cursor_position_callback(GLFWwindow* window, double xmouse, double ymouse);
    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    static double getMouseX() {return mouseX;}
    static double getMouseY() {return mouseY;}
    static bool isLeftMouseButtonPressed() {return mouseToggles[GLFW_MOUSE_BUTTON_LEFT];}
    static bool isRightMouseButtonPressed() {return mouseToggles[GLFW_MOUSE_BUTTON_RIGHT];}
private:
    static double mouseX, mouseY;
    static bool mouseToggles[2];
};


#endif /* Mouse_hpp */
