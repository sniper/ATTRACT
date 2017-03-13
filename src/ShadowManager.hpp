//
//  ShadowManager.hpp
//  ATTRACT
//
//  Created by Cody Rhoads on 3/10/17.
//
//

#ifndef ShadowManager_hpp
#define ShadowManager_hpp

#define GLEW_STATIC
#include <GL/glew.h>

class ShadowManager
{
public:
    ShadowManager();
    virtual ~ShadowManager();
    void init();
    void bindFramebuffer();         // used when creating the texture
    void unbindFramebuffer();
    void bind(GLint handle);        // used when applying the texture
    void unbind();
    void drawDebug();               // used to draw the debug texture
    
    void setUnit(GLint u) { unit = u; }
    GLint getUnit() const { return unit; }
    
private:
    GLint unit;
    GLuint depthMapFBO;
    const GLuint SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
    GLuint depthMap;
    //geometry for texture render
    GLuint quad_VertexArrayID;
    GLuint quad_vertexbuffer;
};

#endif /* ShadowManager_hpp */
