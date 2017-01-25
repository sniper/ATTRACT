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
    static double getMouseX() {return mouseX;}
    static double getMouseY() {return mouseY;}
private:
    static double mouseX, mouseY;
};


#endif /* Mouse_hpp */
