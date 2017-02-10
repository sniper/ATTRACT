//
//  InputManager.hpp
//  Lab1
//
//  Created by Cody Rhoads on 1/18/17.
//
//

#ifndef InputManager_hpp
#define InputManager_hpp

#include <memory>
#include "BulletSphere.h"

class Camera;
class BulletManager;

class InputManager
{
public:
    InputManager(std::shared_ptr<Camera> &camera);
    virtual ~InputManager();
    void processGameInputs(BulletManager *bullet);
private:
    std::shared_ptr<Camera> camera;
};

#endif /* InputManager_hpp */
