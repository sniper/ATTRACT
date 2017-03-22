//
//  Bloom.hpp
//  ATTRACT
//
//  Created by Cody Rhoads on 3/21/17.
//
//

#ifndef Bloom_hpp
#define Bloom_hpp

#include "GLSL.h"

class Bloom
{
public:
    Bloom();
    virtual ~Bloom();
    void init();
    void bindFramebuffer();
    void unbindFramebuffer();
    void bindColor(GLint handle);
    void unbindColor();
    void bindBloom(GLint handle);
    void unbindBloom();
    void renderQuad();
    void gaussianBlur(GLint handle);
    void applyBloom(GLint sceneHandle, GLint bloomHandle);
    
    void setUnit(GLint u) { unit = u; }
    GLint getUnit() const { return unit; }
    
private:
    GLint unit;
    GLboolean horizontal;
    GLuint FBO, pingpongFBO[2];
    const GLuint WIDTH = 1024, HEIGHT = 1024;
    GLuint colorBuffers[2], pingpongBuffers[2];
    GLuint depthBuf;
    //geometry for texture render
    GLuint quad_VertexArrayID;
    GLuint quad_vertexbuffer;
};

#endif /* Bloom_hpp */
