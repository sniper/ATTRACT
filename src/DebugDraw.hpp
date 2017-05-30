//
//  DebugDraw.hpp
//  ATTRACT
//
//  Created by Cody Rhoads on 5/17/17.
//
//

#ifndef DebugDraw_hpp
#define DebugDraw_hpp

#include <memory>
#include <glm/gtc/type_ptr.hpp>

class Shape;
class Program;

class DebugDraw
{
public:
    DebugDraw();
    DebugDraw(const std::shared_ptr<Program> &prog, const std::shared_ptr<Shape> &sphere);
    
    void drawPoint(const glm::vec3 &position, const glm::vec3 &color, const glm::vec3 &scale);
    //void drawLine(const glm::vec3 &from, const glm::vec3 &to, const glm::vec3 &color);
    
    void setPV(const glm::mat4 &newP, const glm::mat4 &newV);
private:
    std::shared_ptr<Program> prog;
    glm::mat4 P;
    glm::mat4 V;
    std::shared_ptr<Shape> sphere;
};

#endif /* DebugDraw_hpp */
