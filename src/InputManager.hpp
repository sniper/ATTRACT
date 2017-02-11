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
#include "GuiManager.hpp"
#include "GameManager.hpp"


class Camera;
class BulletManager;
class GuiManager;

class InputManager
{
public:
    InputManager(std::shared_ptr<Camera> &camera);
    virtual ~InputManager();
    State processGameInputs(BulletManager *bullet);
    State processMenuInputs(GuiManager *gui);
    State processPauseInputs(GuiManager *gui);
private:
    std::shared_ptr<Camera> camera;
};

#endif /* InputManager_hpp */
