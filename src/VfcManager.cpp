/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   VfcManager.cpp
 * Author: root
 * 
 * Created on February 2, 2017, 4:08 PM
 */

#include "VfcManager.hpp"

VfcManager::VfcManager() {
}

VfcManager::VfcManager(const VfcManager& orig) {
}

VfcManager::~VfcManager() {
}

bool VfcManager::viewFrustCull(vector<vec3> *verts) {
    float dist;
    vec3 p, n;

    if (verts->size() == 0)
        cerr << "viewFrustCull IS BEING PASSED AN EMPTY VERTICES VECTOR" << endl;





    for (int i = 0; i < 6; i++) {
        /* get P vert*/
        p = verts->at(0);
        if (planes[i].x >= 0)
            p.x = verts->at(1).x;
        if (planes[i].y >= 0)
            p.y = verts->at(1).y;
        if (planes[i].z >= 0)
            p.z = verts->at(1).z;

        /*get N vert, dont really need this technically*/
        
        /*
        n = verts->at(1);

        if (planes[i].x >= 0)
            n.x = verts->at(0).x;
        if (planes[i].y >= 0)
            n.y = verts->at(0).y;
        if (planes[i].z >= 0)
            n.z = verts->at(0).z;

         */
        
        if(distToPlane(planes[i].x, planes[i].y, planes[i].z, planes[i].w, p)<0)
            return true;


    }
    return false;

}

float VfcManager::distToPlane(float A, float B, float C, float D, vec3 point) {
    return (A * point.x + B * point.y + C * point.z + D);
}

void VfcManager::extractVFPlanes(mat4 P, mat4 V) {

    /* composite matrix */
    mat4 comp = P*V;
    vec3 n; //use to pull out normal
    float l; //length of normal for plane normalization

    Left.x = comp[0][3] + comp[0][0]; // see handout to fill in with values from comp
    Left.y = comp[1][3] + comp[1][0]; // see handout to fill in with values from comp
    Left.z = comp[2][3] + comp[2][0]; // see handout to fill in with values from comp
    Left.w = comp[3][3] + comp[3][0]; // see handout to fill in with values from comp
    n = vec3(Left.x, Left.y, Left.z);
    l = length(n);
    planes[0] = Left = Left / l;

    Right.x = comp[0][3] - comp[0][0]; // see handout to fill in with values from comp
    Right.y = comp[1][3] - comp[1][0]; // see handout to fill in with values from comp
    Right.z = comp[2][3] - comp[2][0]; // see handout to fill in with values from comp
    Right.w = comp[3][3] - comp[3][0]; // see handout to fill in with values from comp
    n = vec3(Right.x, Right.y, Right.z);
    l = length(n);
    planes[1] = Right = Right / l;

    Bottom.x = comp[0][3] + comp[0][1]; // see handout to fill in with values from comp
    Bottom.y = comp[1][3] + comp[1][1]; // see handout to fill in with values from comp
    Bottom.z = comp[2][3] + comp[2][1]; // see handout to fill in with values from comp
    Bottom.w = comp[3][3] + comp[3][1]; // see handout to fill in with values from comp 
    n = vec3(Bottom.x, Bottom.y, Bottom.z);
    l = length(n);
    planes[2] = Bottom = Bottom / l;

    Top.x = comp[0][3] - comp[0][1]; // see handout to fill in with values from comp
    Top.y = comp[1][3] - comp[1][1]; // see handout to fill in with values from comp
    Top.z = comp[2][3] - comp[2][1]; // see handout to fill in with values from comp
    Top.w = comp[3][3] - comp[3][1]; // see handout to fill in with values from comp
    n = vec3(Top.x, Top.y, Top.z);
    l = length(n);
    planes[3] = Top = Top / l;

    Near.x = comp[0][3] + comp[0][2]; // see handout to fill in with values from comp
    Near.y = comp[1][3] + comp[1][2]; // see handout to fill in with values from comp
    Near.z = comp[2][3] + comp[2][2]; // see handout to fill in with values from comp
    Near.w = comp[3][3] + comp[3][2]; // see handout to fill in with values from comp
    n = vec3(Near.x, Near.y, Near.z);
    l = length(n);
    planes[4] = Near = Near / l;

    Far.x = comp[0][3] - comp[0][2]; // see handout to fill in with values from comp
    Far.y = comp[1][3] - comp[1][2]; // see handout to fill in with values from comp
    Far.z = comp[2][3] - comp[2][2]; // see handout to fill in with values from comp
    Far.w = comp[3][3] - comp[3][2]; // see handout to fill in with values from comp
    n = vec3(Far.x, Far.y, Far.z);
    l = length(n);
    planes[5] = Far = Far / l;
}


