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
#include <bullet/BulletCollision/Gimpact/btGImpactCollisionAlgorithm.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

#define PI_F (3.14f)

using namespace std;
using namespace glm;
class BulletManager {
public:
    BulletManager();
    virtual ~BulletManager();
    void createGroundPlane(double x, double y, double z);
    void createSphere(vec3 pos, double radius);
    vec3 stepAndPrint(float dt);
    
    bool getStepFlag();
    void setStepFlag(bool f);
    
    void setJumpDirection(vec3 dir, int key);
    
    
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
    
    bool stepFlag;



};

#endif /* BULLETMANAGER_H */

