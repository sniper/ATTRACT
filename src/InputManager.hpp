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
class GameObject;

class InputManager
{
public:
    InputManager(std::shared_ptr<Camera> &camera);
    virtual ~InputManager();
    
    void setCamera(const std::shared_ptr<Camera> &newCamera) {camera = newCamera;}
    State processGameInputs(std::shared_ptr<BulletManager> &bullet, std::shared_ptr<FmodManager> &fmod);
    State processCutsceneInputs(std::shared_ptr<BulletManager> &bullet, std::shared_ptr<FmodManager> &fmod, std::shared_ptr<GameObject> obj, State gameState,std::shared_ptr<GuiManager> &gui );
    State processMenuInputs(std::shared_ptr<GuiManager> &gui, std::shared_ptr<FmodManager> &fmod);
    State processPauseInputs(std::shared_ptr<GuiManager> &gui, std::shared_ptr<FmodManager> &fmod);
    State processDeathInputs(std::shared_ptr<GuiManager> &gui,std::shared_ptr<FmodManager> &fmod);
    State processWinInputs(std::shared_ptr<GuiManager> &gui,std::shared_ptr<FmodManager> &fmod);

private:
    std::shared_ptr<Camera> camera;
};

#endif /* InputManager_hpp */
