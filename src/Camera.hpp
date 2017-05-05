//
//  Camera.hpp
//  Lab1
//
//  Created by Cody Rhoads on 5/24/16.
//
//

#ifndef Camera_hpp
#define Camera_hpp

#include <memory>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>

class MatrixStack;
class BoundingSphere;
class GameObject;
class BulletManager;
class AABoundingBox;
class FmodManager;

class Camera
{
public:
    Camera();
    Camera(const glm::vec3 &position, GLFWwindow *window);
    virtual ~Camera();
    
    void mouseMoved(double x, double y, bool constrainYaw);
    void interpretPressedKeys(const std::vector<char> &pressedKeys, std::shared_ptr<BulletManager> bullet, std::shared_ptr<FmodManager> fmod);
    void applyProjectionMatrix(std::shared_ptr<MatrixStack> P) const;
    void applyViewMatrix(std::shared_ptr<MatrixStack> MV) const;
    bool checkForCollision(const std::shared_ptr<GameObject> &otherObj);
    void resolveCollision();
    
    void setAspect(float a) {aspect = a;}
    float getAspect() const {return aspect;}
    void setBoundingBox(const std::shared_ptr<AABoundingBox> &box) {boundingBox = box;}
    
    void setPosition(glm::vec3 inPos);
    void setLookingAtMagnet(bool newLookingAtMagnet) {lookingAtMagnet = newLookingAtMagnet;}
    
    glm::vec3 getPosition() const {return position;}
    glm::vec3 getDirection() const {return glm::normalize(glm::vec3(cos(pitch) * cos(yaw), sin(pitch), cos(pitch) * cos((3.14/2) - yaw)));}
    float getRot() const {return pitch;}
    std::shared_ptr<AABoundingBox> getPlayerSphere() const {return boundingBox;}
    bool isLookingAtMagnet() const {return lookingAtMagnet;}
    void setPitch(float in){pitch = in;}
    void setYaw(float in){yaw = in;}
    
    float getFOV() const {return fovy;}
    float getNear() const {return znear;}
    float getFar() const {return zfar;}
private:
    glm::vec3 position;
    glm::vec3 oldPosition;
    GLFWwindow *window;
    float yaw;
    float pitch;
    float aspect;
    float fovy;
    float znear;
    float zfar;
    glm::vec2 mousePrev;
    std::shared_ptr<AABoundingBox> boundingBox;
    bool lookingAtMagnet;
};


#endif /* Camera_hpp */
