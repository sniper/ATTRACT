/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   VfcManager.hpp
 * Author: root
 *
 * Created on February 2, 2017, 4:08 PM
 */

#ifndef VFCMANAGER_HPP
#define VFCMANAGER_HPP


#include <glm/gtc/matrix_transform.hpp> //perspective, trans etc
#include <glm/gtc/type_ptr.hpp> //value_ptr
#include <iostream>
#include <vector>

using namespace std;
using namespace glm;

class VfcManager {
public:
    VfcManager();
    VfcManager(const VfcManager& orig);
    virtual ~VfcManager();
    bool viewFrustCull(vector<vec3> *verts);
    void extractVFPlanes(mat4 P, mat4 V);
private:
    vec4 Left, Right, Bottom, Top, Near, Far;
    vec4 planes[6];
    float distToPlane(float A, float B, float C, float D, vec3 point);

};

#endif /* VFCMANAGER_HPP */

