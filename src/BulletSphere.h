/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   BulletSphere.h
 * Author: root
 *
 * Created on January 30, 2017, 12:20 PM
 */

#ifndef BULLETSPHERE_H
#define BULLETSPHERE_H 
#include "BulletObject.h"

class BulletSphere : public BulletObject {
public:
    BulletSphere(double x, double y, double z, double radius);
    BulletSphere(const BulletSphere& orig);
    virtual ~BulletSphere();
private:

};

#endif /* BULLETSPHERE_H */

