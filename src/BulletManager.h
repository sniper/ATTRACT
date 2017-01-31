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

class BulletObject;

class BulletManager {
public:
    BulletManager();
    virtual ~BulletManager();
    void createPlane(std::string name, double x, double y, double z);
    void createSphere(std::string name, double x, double y, double z, double radius);
    void createBox(std::string name, double x, double y, double z, glm::vec3 dims, glm::vec3 scale, float mass);
    void step(float dt);
    void rayTrace(std::string obj, glm::vec3 startLoc, glm::vec3 endLoc);
    BulletObject* getBulletObject(std::string name);
    glm::vec3 getBulletObjectState(std::string name);
    btDiscreteDynamicsWorld* getDynamicsWorld() {return dynamicsWorld;};
private:
    /*Bullet stuff*/
    btBroadphaseInterface* broadphase;
    btDefaultCollisionConfiguration* collisionConfiguration;
    btCollisionDispatcher* dispatcher;
    btSequentialImpulseConstraintSolver* solver;
    btDiscreteDynamicsWorld* dynamicsWorld;
    
    std::map<std::string, BulletObject*> bulletObjects;
};

#endif /* BULLETMANAGER_H */

