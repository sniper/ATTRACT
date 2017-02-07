/*
 * Handles the Bullet Physics library
 */

/* 
 * File:   BulletManager.cpp
 * Author: Darryl Vo
 * 
 * Created on January 27, 2017, 4:08 PM
 */
//TODO: if you call the create functions again, it will create memleak. remember to make deconstructor functions too.
// also find out how to remove stuff from the simulator?
#include "BulletManager.hpp"
#include "Mouse.hpp"
#include "BulletPlane.h"
#include "BulletSphere.h"
#include "BulletBox.hpp"

#include <algorithm>

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
    
    /*create debug*/
    debug = new BulletDebugDraw();
    debugFlag = false;
    debug->setDebugMode(btIDebugDraw::DBG_DrawWireframe);
    dynamicsWorld->setDebugDrawer(debug);
}

BulletManager::~BulletManager() {
    /*Bullet doesnt auto deallocate automatically*/

     for (std::map<string,BulletObject*>::iterator it=bulletObjects.begin(); it!=bulletObjects.end(); ++it) {
         dynamicsWorld->removeRigidBody(it->second->getRigidBody());
         delete it->second;
     }

    delete dynamicsWorld;
    delete solver;
    delete dispatcher;
    delete collisionConfiguration;
    delete broadphase;
}

bool BulletManager::getDebugFlag() {
    return debugFlag;
}

void BulletManager::setDebugFlag(bool f) {
    debugFlag = f;
}

void BulletManager::setDebugMode(int mode) {
    debug->setDebugMode(mode);
}

void BulletManager::renderDebug(mat4 p, mat4 v) {
    ((BulletDebugDraw*)debug)->setPV(p,v);
    dynamicsWorld->debugDrawWorld();
}

void BulletManager::createPlane(string name, double x, double y, double z) {

    /*
    btTransform trans; //position and rotation
    trans.setIdentity();
    trans.setOrigin(btVector3(x, y, z));
    
    
    groundShape = new btStaticPlaneShape(btVector3(0, 1, 0), 0);
    groundMotionState = new btDefaultMotionState(
            trans);
    btRigidBody::btRigidBodyConstructionInfo
    groundRigidBodyCI(0, groundMotionState, groundShape, btVector3(0, 0, 0));
    groundRigidBodyCI.m_friction = 0.9f;
    groundRigidBodyCI.m_restitution = 0.5f;
    groundRigidBody = new btRigidBody(groundRigidBodyCI);
    dynamicsWorld->addRigidBody(groundRigidBody);
    */
    BulletObject *ground = new BulletPlane(x, y, z); 
    dynamicsWorld->addRigidBody(ground->getRigidBody());
    bulletObjects.insert(make_pair(name, ground));
}

void BulletManager::createSphere(string name, double x, double y, double z, double radius) {
    /*
    btTransform trans; //position and rotation
    trans.setIdentity();
    trans.setOrigin(btVector3(x, y, z));
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
     */
    BulletObject *sphere = new BulletSphere(x, y, z, radius);
    dynamicsWorld->addRigidBody(sphere->getRigidBody());
    bulletObjects.insert(make_pair(name, sphere));

}

void BulletManager::createBox(string name, vec3 loc, vec3 dims, vec3 scale, float mass) {
    BulletObject *box = new BulletBox(loc, dims, scale, mass);
    dynamicsWorld->addRigidBody(box->getRigidBody());
    bulletObjects.insert(make_pair(name, box));
}

void BulletManager::createMagneticBox(string name, vec3 loc, vec3 dims, vec3 scale, float mass) {
    BulletObject *box = new BulletBox(loc, dims, scale, mass);
    dynamicsWorld->addRigidBody(box->getRigidBody());
    bulletObjects.insert(make_pair(name, box));
    magneticObjects.push_back(box->getRigidBody());
}

void BulletManager::step(float dt) {
    dynamicsWorld->stepSimulation(dt);
}

BulletObject* BulletManager::getBulletObject(string name) {
    return bulletObjects[name];
}

vec3 BulletManager::getBulletObjectState(string name) {
    btTransform trans;
    vec3 ret;
    bulletObjects[name]->getRigidBody()->getMotionState()->getWorldTransform(trans);
    ret.x = trans.getOrigin().getX();
    ret.y = trans.getOrigin().getY();
    ret.z = trans.getOrigin().getZ();
    //cout << "x:" << ret.x << " y:" << ret.y << " z:" << ret.z << endl;
    return ret;
}

/*
void BulletManager::rayTrace(vec3 startLoc, vec3 endLoc) {
    // Converting glm vectors to bullet vectors.
    btVector3 start = btVector3(startLoc[0], startLoc[1], startLoc[2]);
    btVector3 end = btVector3(endLoc[0], endLoc[1], endLoc[2]);
    
    // Finds the closest object from the start location to the end location.
    btCollisionWorld::ClosestRayResultCallback RayCallback(start, end);
    dynamicsWorld->rayTest(start, end, RayCallback);

    // Get the object that is hit.
    const btRigidBody *hitShape = (btRigidBody *) RayCallback.m_collisionObject;

    // We will eventually have to do a check to make sure the object is a magnetic surface.
    // For now, just check to see if its looking at the bunny.
    if (RayCallback.hasHit() &&
        find(magneticObjects.begin(), magneticObjects.end(), hitShape) != magneticObjects.end()) {
        lookingAtMagnet = true;
        //cout << "HIT" << endl;
        if (Mouse::isLeftMouseButtonPressed()) {
            vec3 dir = normalize(endLoc - startLoc);
            btVector3 bulletDir = btVector3(dir.x, dir.y, dir.z);
            bulletObjects["cam"]->getRigidBody()->setLinearVelocity(bulletDir * MAGNET_STRENGTH);
            //bulletObjects["cam"]->getRigidBody()->setGravity(btVector3(0.0f, 0.0f, 0.0f));
        }
        else if (Mouse::isRightMouseButtonPressed()) {
            vec3 dir = normalize(startLoc - endLoc);
            btVector3 bulletDir = btVector3(dir.x, dir.y, dir.z);
            bulletObjects["cam"]->getRigidBody()->setLinearVelocity(bulletDir * MAGNET_STRENGTH);
            //bulletObjects["cam"]->getRigidBody()->setGravity(btVector3(0.0f, 0.0f, 0.0f));
        }
        else {
            bulletObjects["cam"]->getRigidBody()->setGravity(dynamicsWorld->getGravity());
        }
    }
    else {
        lookingAtMagnet = false;
        bulletObjects["cam"]->getRigidBody()->setGravity(dynamicsWorld->getGravity());
    }
}
*/


