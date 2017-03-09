#pragma once
#ifndef _SHAPE_H_
#define _SHAPE_H_

#include <string>
#include <vector>
#include <memory>
#include <glm/gtc/type_ptr.hpp>

class Program;
class Texture;

class Shape
{
public:
    Shape();
    virtual ~Shape();
    void loadMesh(const std::string &meshName, const std::string &resourceDir);
    void fitToUnitBox();
    void init();
    void draw(const std::shared_ptr<Program> prog) const;
    
private:
    std::string RESOURCE_DIR;
    
    std::vector<std::vector<unsigned int>> eleBufs;
    std::vector<float> posBuf;
    std::vector<float> norBuf;
    std::vector<float> texBuf;
    std::vector<float> tangentBuf;
    std::vector<float> bitangentBuf;
    std::vector<int> matIDBuf;
    std::vector<glm::vec3> ambientBuf;
    std::vector<glm::vec3> diffuseBuf;
    std::vector<glm::vec3> specularBuf;
    std::vector<std::string> diffuseTexNameBuf;
    std::vector<std::string> normalTexNameBuf;
    
    std::vector<unsigned> eleBufID;
    unsigned posBufID;
    unsigned norBufID;
    unsigned texBufID;
    unsigned tangentBufID;
    unsigned bitangentBufID;
    unsigned vao;
    
    std::vector<std::shared_ptr<Texture>> diffuseTextures;
    std::vector<std::shared_ptr<Texture>> normalTextures;
};

#endif
