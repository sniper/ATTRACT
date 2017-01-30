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

using namespace std;
using namespace glm;
class BulletManager {
public:
    BulletManager();
    virtual ~BulletManager();
    void createGroundPlane(double x, double y, double z);
    void createSphere(double x, double y, double z, double radius);
    vec3 stepAndPrint(float dt);
    void rayTrace(glm::vec3 startLoc, glm::vec3 endLoc);
private:
    /*Bullet stuff*/
    btBroadphaseInterface* broadphase;
    btDefaultCollisionConfiguration* collisionConfiguration;
    btCollisionDispatcher* dispatcher;
    btSequentialImpulseConstraintSolver* solver;
    btDiscreteDynamicsWorld* dynamicsWorld;

    /*Collision shapes*/
    btCollisionShape* groundShape;
    btCollisionShape* sphereShape;

    /*motion states */
    btDefaultMotionState* groundMotionState;
    btDefaultMotionState* sphereMotionState;

    /*rigid bodies*/
    btRigidBody* groundRigidBody;
    btRigidBody* sphereRigidBody;



};

#endif /* BULLETMANAGER_H */

