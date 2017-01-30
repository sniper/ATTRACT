/* 
 * File:   BulletManager.h
 * Author: Darryl
 *
 * Created on January 27, 2017, 4:08 PM
 */

#ifndef BULLETMANAGER_H
#define BULLETMANAGER_H

#include <iostream>
#include <btBulletDynamicsCommon.h>
#include <BulletCollision/Gimpact/btGImpactCollisionAlgorithm.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <map>
#include "BulletObject.h"
#include "BulletPlane.h"
#include "BulletSphere.h"
using namespace std;
using namespace glm;
class BulletManager {
public:
    BulletManager();
    virtual ~BulletManager();
    void createPlane(string name, double x, double y, double z);
    void createSphere(string name, double x, double y, double z, double radius);
    void step(float dt);
    void rayTrace(string obj, glm::vec3 startLoc, glm::vec3 endLoc);
    BulletObject* getBulletObject(string name);
    vec3 getBulletObjectState(string name);
private:
    /*Bullet stuff*/
    btBroadphaseInterface* broadphase;
    btDefaultCollisionConfiguration* collisionConfiguration;
    btCollisionDispatcher* dispatcher;
    btSequentialImpulseConstraintSolver* solver;
    btDiscreteDynamicsWorld* dynamicsWorld;
    
    map<string,BulletObject*> bulletObjects;




};

#endif /* BULLETMANAGER_H */

