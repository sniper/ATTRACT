/* 
 * File:   BulletDebugDraw.cpp
 * Author: Darryl
 * 
 * Created on February 1, 2017, 12:27 PM
 */

#include "BulletDebugDraw.hpp"
#include "GameManager.hpp"

BulletDebugDraw::BulletDebugDraw() {
}

BulletDebugDraw::BulletDebugDraw(const BulletDebugDraw& orig) {
}

BulletDebugDraw::~BulletDebugDraw() {
}

void BulletDebugDraw::drawLine(const btVector3 &from, const btVector3 &to, const btVector3 &color) {
    float fromX = from.x();
    float fromY = from.y();
    float fromZ = from.z();
    vec4 from4 = vec4(fromX, fromY, fromZ, 1.0f);
    vec4 to4 = vec4((float) to.x(), (float) to.y(), (float) to.z(), 1.0f);

    from4 = p * v * from4;
    to4 = p * v * to4;
    
    glDisable(GL_DEPTH_TEST);
    glClear(GL_DEPTH_BUFFER_BIT);

    glLineWidth(2.5);
    glColor3f(color.x(), color.y(), color.z());
    glBegin(GL_LINES);
    glVertex3f(to4.x, to4.y, to4.z);
    glVertex3f(from4.x, from4.y, from4.z);
    glEnd();

    glEnable(GL_DEPTH_TEST);
}

void BulletDebugDraw::setPV(mat4 inP, mat4 inV) {
    p = inP;
    v = inV;
}

void BulletDebugDraw::drawContactPoint(const btVector3 &PointOnB, const btVector3 &normalOnB, btScalar distance, int lifeTime, const btVector3 &color) {
    cout << "unimplmeneted draw contact point" << endl;
}

void BulletDebugDraw::reportErrorWarning(const char *warningString) {
    cerr << "error warning: " << warningString << endl;
}

void BulletDebugDraw::draw3dText(const btVector3 &location, const char *textString) {
    cout << "3d text: " << textString << endl;
}

void BulletDebugDraw::setDebugMode(int debugMode) {
    mode = debugMode;
}

int BulletDebugDraw::getDebugMode() const {
    return mode;
}