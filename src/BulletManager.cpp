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
//TODO: if you call the create functions again, it will create memleak. remmber to make deconstructor functions too.
// also find out how to remove stuff from the simulator?
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
}

BulletManager::~BulletManager() {
    /*Bullet doesnt auto deallocate automatically*/

    delete groundShape;
    delete sphereShape;
    delete groundMotionState;
    delete groundRigidBody;
    delete sphereRigidBody;
    delete sphereMotionState;

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

void BulletManager::createSphere(double x, double y, double z, double radius) {
    sphereShape = new btSphereShape(radius);
    sphereMotionState = new btDefaultMotionState(btTransform(
            btQuaternion(0, 0, 0, 1), btVector3(x, y, z)));
    btScalar mass = 1;
    btVector3 sphereInertia(0, 0, 0);
    sphereShape->calculateLocalInertia(mass, sphereInertia);
    btRigidBody::btRigidBodyConstructionInfo sphereRigidBodyCI(mass, sphereMotionState, sphereShape, sphereInertia);
    sphereRigidBody = new btRigidBody(sphereRigidBodyCI);
    dynamicsWorld->addRigidBody(sphereRigidBody);

}

vec3 BulletManager::stepAndPrint(float dt) {
    if(sphereRigidBody == NULL) {
        cerr << "you didnt initalize the bullet correctly" << endl;
        exit(1);
    }
    dynamicsWorld->stepSimulation(dt, 10);

    btTransform trans;
    sphereRigidBody->getMotionState()->getWorldTransform(trans);

    std::cout << "sphere height: " << trans.getOrigin().getY() << std::endl;
    return vec3(trans.getOrigin().getX(), trans.getOrigin().getY(), trans.getOrigin().getZ());

}



