//
//  BulletBox.hpp
//  ATTRACT
//
//  Created by Cody Rhoads on 1/30/17.
//
//

#ifndef BulletBox_hpp
#define BulletBox_hpp

#include "BulletObject.h"

class BulletBox : public BulletObject {
public:
    BulletBox(double x, double y, double z, glm::vec3 dims, glm::vec3 scale, float mass);
    BulletBox(const BulletBox& orig);
    virtual ~BulletBox();
};

#endif /* BulletBox_hpp */
