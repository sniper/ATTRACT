//
//  Material.hpp
//  Lab1
//
//  Created by Cody Rhoads on 5/6/16.
//
//

#ifndef Material_hpp
#define Material_hpp

#include <memory>

class Program;

class Material
{
public:
    Material();
    Material(const glm::vec3 &ka, const glm::vec3 &kd, const glm::vec3 &ks, float s);
    virtual ~Material();
    void setUniforms(const std::shared_ptr<Program> &prog);
private:
    glm::vec3 ka;
    glm::vec3 kd;
    glm::vec3 ks;
    float s;
};

#endif /* Material_hpp */
