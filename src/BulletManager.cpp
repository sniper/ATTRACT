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
//TODO: if you call the create functions again, it will create memleak. remember to make deconstructor functions too.
// also find out how to remove stuff from the simulator?
#include "BulletManager.h"
#include "Mouse.hpp"

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
    groundRigidBodyCI.m_friction = 0.9f;
    groundRigidBodyCI.m_restitution = 0.5f;
    groundRigidBody = new btRigidBody(groundRigidBodyCI);
    dynamicsWorld->addRigidBody(groundRigidBody);
}

void BulletManager::createSphere(double x, double y, double z, double radius) {
    btTransform trans;	//position and rotation
    trans.setIdentity();
    trans.setOrigin(btVector3(x,y,z));
    sphereShape = new btSphereShape(radius);
    sphereMotionState = new btDefaultMotionState(trans);
    btScalar mass = 1.0f;
    btVector3 sphereInertia(0.0f, 0.0f, 0.0f);
    sphereShape->calculateLocalInertia(mass, sphereInertia);
    btRigidBody::btRigidBodyConstructionInfo sphereRigidBodyCI(mass, sphereMotionState, sphereShape, sphereInertia);
    sphereRigidBodyCI.m_friction = 0.9f;
    sphereRigidBodyCI.m_restitution = 0.5f;
    sphereRigidBodyCI.m_rollingFriction = 0.1f;
    sphereRigidBody = new btRigidBody(sphereRigidBodyCI);
    dynamicsWorld->addRigidBody(sphereRigidBody);

}

vec3 BulletManager::stepAndPrint(float dt) {
    if (sphereRigidBody == NULL) {
        cerr << "Bullet initialized incorrectly" << endl;
        exit(1);
    }
    dynamicsWorld->stepSimulation(dt);

    if (sphereRigidBody->isActive()) {
        cout << "sphere is active" << endl;
    }
    
    btTransform trans;
    sphereRigidBody->getMotionState()->getWorldTransform(trans);

    std::cout << "sphere height: " << trans.getOrigin().getY() << std::endl;
    return vec3(trans.getOrigin().getX(), trans.getOrigin().getY(), trans.getOrigin().getZ());
}

void BulletManager::rayTrace(vec3 startLoc, vec3 endLoc) {
    // Converting glm vectors to bullet vectors.
    btVector3 start = btVector3(startLoc[0], startLoc[1], startLoc[2]);
    btVector3 end = btVector3(endLoc[0], endLoc[1], endLoc[2]);
    
    // Finds the closest object from the start location to the end location.
    btCollisionWorld::ClosestRayResultCallback RayCallback(start, end);
    dynamicsWorld->rayTest(start, end, RayCallback);
    
    // Get the object that is hit.
    const btRigidBody *hitShape = (btRigidBody *)RayCallback.m_collisionObject;
    
    // We will eventually have to do a check to make sure the object is a magnetic surface.
    // For now, just check to see if its looking at the bunny.
    if (RayCallback.hasHit() && hitShape == sphereRigidBody) {
        if (Mouse::isLeftMouseButtonPressed()) {
            sphereRigidBody->activate();
            sphereRigidBody->setGravity(start - end);
        }
        else if (Mouse::isRightMouseButtonPressed()) {
            sphereRigidBody->activate();
            sphereRigidBody->setGravity(end - start);
        }
        else {
            sphereRigidBody->setGravity(dynamicsWorld->getGravity());
        }
    }
    else {
        sphereRigidBody->setGravity(dynamicsWorld->getGravity());
    }
}



