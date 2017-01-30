/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   BulletPlane.h
 * Author: root
 *
 * Created on January 30, 2017, 10:42 AM
 */

#ifndef BULLETPLANE_H
#define BULLETPLANE_H

#include "BulletObject.h"

class BulletPlane : public BulletObject {
public:
    BulletPlane(double x, double y, double z);
    BulletPlane(const BulletPlane& orig);
    virtual ~BulletPlane();
private:

};

#endif /* BULLETPLANE_H */

