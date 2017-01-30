/*
 * Handles the Bullet Physics library:
 * Will not handle collision, we just use this to
 * simulate motion. 
 */

/* 
 * File:   BulletManager.cpp
 * Author: Darryl Vo
 * 
 * Created on January 27, 2017, 4:08 PM
 */
//TODO: MEME LEAK MEME LEAK MEME LEAKs
#include "BulletManager.h"

BulletManager::BulletManager() {

    /* Initialize all the bullet stuff*/
    broadphase = new btDbvtBroadphase();
    collisionConfiguration = new btDefaultCollisionConfiguration();
    dispatcher = new btCollisionDispatcher(collisionConfiguration);
    solver = new btSequentialImpulseConstraintSolver;
    btGImpactCollisionAlgorithm::registerAlgorithm(dispatcher);
    /*the main world*/
    dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);

    /*set gravity*/
    dynamicsWorld->setGravity(btVector3(0, -10, 0));

    /*null out the rest of the pointers*/
    groundShape = NULL;
    sphereShape = NULL;
    groundMotionState = NULL;
    sphereMotionState = NULL;
    groundRigidBody = NULL;
    sphereRigidBody = NULL;

    stepFlag = false;
    
    createSphere(vec3(0,0,0), 2);
}

BulletManager::~BulletManager() {
    /*Bullet doesnt auto deallocate automatically*/

    //delete groundShape;
    //delete sphereShape;
    //delete groundMotionState;
    //delete groundRigidBody;
    //delete sphereRigidBody;
    //delete sphereMotionState;

    delete dynamicsWorld;
    delete solver;
    delete dispatcher;
    delete collisionConfiguration;
    delete broadphase;
}

void BulletManager::createGroundPlane(double x, double y, double z) {
    groundShape = new btStaticPlaneShape(btVector3(x, y, z), 1);
    groundMotionState = new btDefaultMotionState(
            btTransform(btQuaternion(0, 0, 0, 1), btVector3(x, -y, z)));
    btRigidBody::btRigidBodyConstructionInfo
    groundRigidBodyCI(0, groundMotionState, groundShape, btVector3(0, 0, 0));
    groundRigidBody = new btRigidBody(groundRigidBodyCI);
    dynamicsWorld->addRigidBody(groundRigidBody);
}

void BulletManager::createSphere(vec3 pos, double radius) {


    sphereShape = new btSphereShape(radius);
    sphereMotionState = new btDefaultMotionState(btTransform(
            btQuaternion(0, 0, 0, 1), btVector3(pos.x, pos.y, pos.z)));
    btScalar mass = 1;
    btVector3 sphereInertia(0, 0, 0);
    sphereShape->calculateLocalInertia(mass, sphereInertia);
    btRigidBody::btRigidBodyConstructionInfo sphereRigidBodyCI(mass, sphereMotionState, sphereShape, sphereInertia);
    sphereRigidBody = new btRigidBody(sphereRigidBodyCI);

    dynamicsWorld->addRigidBody(sphereRigidBody);
}

void BulletManager::setJumpDirection(vec3 dir, int key) {
    vec3 newVec;
    dir.y = 0;
    dir = normalize(dir);
    switch (key) {
        case 'w':
            cout << "jump forward" << endl;
            sphereRigidBody->setLinearVelocity(btVector3(dir.x * 2, 3, dir.z * 2));
            break;
        case 'w' + 'a':
            cout << "jump up left" << endl;
            newVec = rotateY(dir, 45.0f * PI_F/180);
            sphereRigidBody->setLinearVelocity(btVector3(newVec.x * 2, 3, newVec.z * 2));
            break;
        case 'a':
            cout << "jump left" << endl;
            newVec = rotateY(dir, PI_F/2.0f);
            sphereRigidBody->setLinearVelocity(btVector3(newVec.x * 2, 3, newVec.z * 2));
            break;
        case 'a' + 's':
            cout << "jump back left" << endl;
            newVec = rotateY(dir, -225.0f * PI_F/180);
            sphereRigidBody->setLinearVelocity(btVector3(newVec.x * 2, 3, newVec.z * 2));
            break;

        case 's':
            cout << "jump backwords" << endl;
            
            sphereRigidBody->setLinearVelocity(btVector3(dir.x * -2, 3, dir.z * -2));
            break;

        case 's' + 'd':
            newVec = rotateY(dir, 225.0f * PI_F/180);
            sphereRigidBody->setLinearVelocity(btVector3(newVec.x * 2, 3, newVec.z * 2));
            break;

        case 'd':
            newVec = rotateY(dir, -90.0f * PI_F/180);
            sphereRigidBody->setLinearVelocity(btVector3(newVec.x * 2, 3, newVec.z * 2));
            break;

        case 'w' + 'd':
            newVec = rotateY(dir, -45.0f * PI_F/180);
            sphereRigidBody->setLinearVelocity(btVector3(newVec.x * 2, 3, newVec.z * 2));
            break;
        default:
            newVec = rotateY(dir, 90.0f);
            sphereRigidBody->setLinearVelocity(btVector3(0, 3, 0));
            break;
    }
}

vec3 BulletManager::stepAndPrint(float dt) {
    /*error checking...*/
    if (sphereRigidBody == NULL) {
        cerr << "you didnt initalize the bullet correctly" << endl;
        exit(1);
    }


    dynamicsWorld->stepSimulation(dt, 10);

    btTransform trans;
    sphereRigidBody->getMotionState()->getWorldTransform(trans);

    std::cout << "sphere height: " << trans.getOrigin().getY() << std::endl;
    return vec3(trans.getOrigin().getX(), trans.getOrigin().getY(), trans.getOrigin().getZ());

}

bool BulletManager::getStepFlag() {
    return stepFlag;
}

void BulletManager::setStepFlag(bool f) {
    stepFlag = f;
    if (f == false && sphereRigidBody != NULL) {
        dynamicsWorld->removeRigidBody(sphereRigidBody);
        delete sphereRigidBody;
        delete sphereMotionState;
        delete sphereShape;

        sphereRigidBody = NULL;
        sphereMotionState = NULL;
        sphereShape = NULL;
    }
}



