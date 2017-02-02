/* 
 * File:   BulletDebugDraw.hpp
 * Author: Darryl
 *
 * Created on February 1, 2017, 12:27 PM
 */

#ifndef BULLETDEBUGDRAW_HPP
#define BULLETDEBUGDRAW_HPP

#include <btBulletDynamicsCommon.h>
#include <LinearMath/btIDebugDraw.h>
#include <iostream>
#include "MatrixStack.h"
using namespace std;

class BulletDebugDraw : public btIDebugDraw {
public:
    BulletDebugDraw();
    BulletDebugDraw(const BulletDebugDraw& orig);
    virtual ~BulletDebugDraw();
    void drawLine(const btVector3 &from, const btVector3 &to, const btVector3 &color);
    void drawContactPoint(const btVector3 &PointOnB, const btVector3 &normalOnB, btScalar distance, int lifeTime, const btVector3 &color);
    void reportErrorWarning(const char *warningString);
    void draw3dText(const btVector3 &location, const char *textString);
    void setDebugMode(int debugMode);
    int getDebugMode() const;
    void setPV(mat4 p, mat4 v);
private:
    int mode;
    mat4 p;
    mat4 v;
    
};

#endif /* BULLETDEBUGDRAW_HPP */

