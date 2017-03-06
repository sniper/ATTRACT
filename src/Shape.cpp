#include "Shape.h"
#include <iostream>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "GLSL.h"
#include "Program.h"
#include "Texture.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

using namespace std;
using namespace glm;

Shape::Shape() :
eleBufID(0),
posBufID(0),
norBufID(0),
texBufID(0) {

}

Shape::~Shape() {

}

void Shape::loadMesh(const string &meshName, const string &resourceDir) {
    // Load geometry
    // Some obj files contain material information.
    vector<tinyobj::shape_t> shapes;
    vector<tinyobj::material_t> objMaterials;
    string errStr;
    
    RESOURCE_DIR = resourceDir;
    
    bool rc = tinyobj::LoadObj(shapes, objMaterials, errStr, meshName.c_str(),
                               resourceDir.c_str());
    if (!rc) {
        cerr << errStr << endl;
    } else {
        vector<unsigned int> tempEleBuf;
        
        posBuf = shapes[0].mesh.positions;
        norBuf = shapes[0].mesh.normals;
        texBuf = shapes[0].mesh.texcoords;
        tempEleBuf = shapes[0].mesh.indices;
        matIDBuf = shapes[0].mesh.material_ids;
        
        for (int i = 0; i < objMaterials.size(); i++) {
            ambientBuf.push_back(vec3(objMaterials[i].ambient[0],
                                      objMaterials[i].ambient[1],
                                      objMaterials[i].ambient[2]));
            diffuseBuf.push_back(vec3(objMaterials[i].diffuse[0],
                                      objMaterials[i].diffuse[1],
                                      objMaterials[i].diffuse[2]));
            specularBuf.push_back(vec3(objMaterials[i].specular[0],
                                       objMaterials[i].specular[1],
                                       objMaterials[i].specular[2]));
            diffuseTexNameBuf.push_back(objMaterials.at(i).diffuse_texname);
        }
        
        if (matIDBuf[0] != -1) {
            int startBuf = 0, endBuf = 0;
            int oldMatID = matIDBuf[0];
    
            while (endBuf != matIDBuf.size() + 1) {
                while (endBuf != matIDBuf.size() + 1 &&
                       matIDBuf[endBuf] == oldMatID) {
                    endBuf++;
                }
                
                if (endBuf == matIDBuf.size() + 1) {
                    break;
                }
                
                eleBufs.push_back(vector<unsigned int>(tempEleBuf.begin() + startBuf*3,
                                                       tempEleBuf.begin() + endBuf*3));
                
                startBuf = endBuf;
                oldMatID = matIDBuf[endBuf];
            }
        }
        else {
            eleBufs.push_back(tempEleBuf);
        }
    }
}

void Shape::fitToUnitBox() {
    // Scale the vertex positions so that they fit within [-1, +1] in all three dimensions.
    glm::vec3 vmin(posBuf[0], posBuf[1], posBuf[2]);
    glm::vec3 vmax(posBuf[0], posBuf[1], posBuf[2]);
    for (int i = 0; i < (int) posBuf.size(); i += 3) {
        glm::vec3 v(posBuf[i], posBuf[i + 1], posBuf[i + 2]);
        vmin[0] = std::min(vmin[0], v[0]);
        vmin[1] = std::min(vmin[1], v[1]);
        vmin[2] = std::min(vmin[2], v[2]);
        vmax[0] = std::max(vmax[0], v[0]);
        vmax[1] = std::max(vmax[1], v[1]);
        vmax[2] = std::max(vmax[2], v[2]);
    }
    glm::vec3 center = 0.5f * (vmin + vmax);
    glm::vec3 diff = vmax - vmin;
    float diffmax = diff[0];
    diffmax = std::max(diffmax, diff[1]);
    diffmax = std::max(diffmax, diff[2]);
    float scale = 1.0f / diffmax;
    for (int i = 0; i < (int) posBuf.size(); i += 3) {
        posBuf[i ] = (posBuf[i ] - center[0]) * scale;
        posBuf[i + 1] = (posBuf[i + 1] - center[1]) * scale;
        posBuf[i + 2] = (posBuf[i + 2] - center[2]) * scale;
    }
}

void Shape::init() {

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    
    // Send the position array to the GPU
    glGenBuffers(1, &posBufID);
    glBindBuffer(GL_ARRAY_BUFFER, posBufID);
    glBufferData(GL_ARRAY_BUFFER, posBuf.size() * sizeof (float), &posBuf[0], GL_STATIC_DRAW);

    // Send the normal array to the GPU
    if (!norBuf.empty()) {
        glGenBuffers(1, &norBufID);
        glBindBuffer(GL_ARRAY_BUFFER, norBufID);
        glBufferData(GL_ARRAY_BUFFER, norBuf.size() * sizeof (float), &norBuf[0], GL_STATIC_DRAW);
    }

    // Send the texture array to the GPU
    if (!texBuf.empty()) {
        glGenBuffers(1, &texBufID);
        glBindBuffer(GL_ARRAY_BUFFER, texBufID);
        glBufferData(GL_ARRAY_BUFFER, texBuf.size() * sizeof (float), &texBuf[0], GL_STATIC_DRAW);
    }

    // Send the element arrays to the GPU
    // Also sets up textures if it has them
    unsigned tempBufID;
    shared_ptr<Texture> tempTex;
    for (int i = 0; i < eleBufs.size(); i++) {
        glGenBuffers(1, &tempBufID);
        eleBufID.push_back(tempBufID);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eleBufID[i]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, eleBufs[i].size() * sizeof(unsigned int), &eleBufs[i][0], GL_STATIC_DRAW);
        
        if (i < diffuseTexNameBuf.size() && diffuseTexNameBuf[i] != "") {
            tempTex = make_shared<Texture>();
            tempTex->setFilename(RESOURCE_DIR + diffuseTexNameBuf[i]);
            tempTex->init();
            tempTex->setUnit(i);
            tempTex->setWrapModes(GL_REPEAT, GL_REPEAT);
            diffuseTextures.push_back(tempTex);
        }
    }

    // Unbind the arrays
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    GLSL::checkError(GET_FILE_LINE);
}

void Shape::draw(const shared_ptr<Program> prog) const {
    // Bind position buffer
    int h_pos = prog->getAttribute("aPos");
    glEnableVertexAttribArray(h_pos);
    glBindBuffer(GL_ARRAY_BUFFER, posBufID);
    glVertexAttribPointer(h_pos, 3, GL_FLOAT, GL_FALSE, 0, (const void *) 0);
    GLSL::checkError(GET_FILE_LINE);
    
    // Bind normal buffer
    int h_nor = prog->getAttribute("aNor");
    if (h_nor != -1 && norBufID != 0) {
        glEnableVertexAttribArray(h_nor);
        glBindBuffer(GL_ARRAY_BUFFER, norBufID);
        glVertexAttribPointer(h_nor, 3, GL_FLOAT, GL_FALSE, 0, (const void *) 0);
    }
    GLSL::checkError(GET_FILE_LINE);

    // Bind texcoords buffer
    int h_tex = prog->getAttribute("aTex");
    if (h_tex != -1 && texBufID != 0) {
        glEnableVertexAttribArray(h_tex);
        glBindBuffer(GL_ARRAY_BUFFER, texBufID);
        glVertexAttribPointer(h_tex, 2, GL_FLOAT, GL_FALSE, 0, (const void *) 0);
    }
    GLSL::checkError(GET_FILE_LINE);
    
    for (int i = 0; i < eleBufs.size(); i++) {
        if (i < ambientBuf.size()) {
            glUniform3f(prog->getUniform("ka"), ambientBuf[i][0],
                        ambientBuf[i][1], ambientBuf[i][2]);
        }
        if (i < diffuseBuf.size()) {
            glUniform3f(prog->getUniform("kd"), diffuseBuf[i][0],
                        diffuseBuf[i][1], diffuseBuf[i][2]);
        }
        if (i < specularBuf.size()) {
            glUniform3f(prog->getUniform("ks"), specularBuf[i][0],
                        specularBuf[i][1], specularBuf[i][2]);
        }
        if (i < diffuseTextures.size() && diffuseTexNameBuf[i] != "") {
            diffuseTextures[i]->bind(prog->getUniform("diffuseTex"));
        }
        
        // Bind element buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eleBufID[i]);

        // Draw
        glDrawElements(GL_TRIANGLES, (int) eleBufs[i].size(), GL_UNSIGNED_INT, (const void *) 0);
    }

    // Disable and unbind
    if (h_tex != -1) {
        glDisableVertexAttribArray(h_tex);
    }
    if (h_nor != -1) {
        glDisableVertexAttribArray(h_nor);
    }
    glDisableVertexAttribArray(h_pos);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    GLSL::checkError(GET_FILE_LINE);
}
