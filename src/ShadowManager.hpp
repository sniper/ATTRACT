//
//  ShadowManager.hpp
//  ATTRACT
//
//  Created by Cody Rhoads on 3/6/17.
//
//

#ifndef ShadowManager_hpp
#define ShadowManager_hpp

#include <memory>
#include "GLSL.h"

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Program;

class ShadowManager
{
public:
    ShadowManager(const std::string &RESOURCE_DIR);
    virtual ~ShadowManager();
    void bindDepthMap(const glm::vec3 &lightDir);
    void unbindDepthMap();
    void addShadowsToScene(const std::shared_ptr<Program> &shader);
    
    std::shared_ptr<Program> getShadowDepthProgram() const {return depthProg;}
private:
    glm::mat4 SetOrthoMatrix();
    glm::mat4 SetLightView(glm::vec3 pos, glm::vec3 LA, glm::vec3 up);
    
    std::shared_ptr<Program> depthProg;
    glm::mat4 LSpace;
    GLuint depthMapFBO;
    GLuint depthMap;
    const GLuint S_WIDTH = 1024, S_HEIGHT = 1024;
};

#endif /* ShadowManager_hpp */
