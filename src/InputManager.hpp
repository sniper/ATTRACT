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
    State processGameInputs(std::shared_ptr<BulletManager> bullet, std::shared_ptr<FmodManager> fmod);
    State processMenuInputs(std::shared_ptr<GuiManager> gui, std::shared_ptr<FmodManager> fmod);
    State processPauseInputs(std::shared_ptr<GuiManager> gui, std::shared_ptr<FmodManager> fmod);
private:
    std::shared_ptr<Camera> camera;
};

#endif /* InputManager_hpp */
