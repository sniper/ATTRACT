//
//  GameManager.hpp
//  Lab1
//
//  Created by Cody Rhoads on 1/17/17.
//
//

#ifndef GameManager_hpp
#define GameManager_hpp

#include <string>
#include <memory>
#include <vector>
#include <map>

#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>
#include "GameObject.hpp"

#define NUM_SHADOW_CASCADES 3
#define NUMLEVELS 7

class InputManager;
class Camera;
class Program;
class Shape;
class GameObject;
class Texture;
class BulletManager;
class VfcManager;
class FmodManager;
class SpaceShipPart;
class ParticleManager;
class Skybox;
class MatrixStack;
class ShadowManager;
class Bloom;

class GuiManager;

enum State {
    GAME, MENU, PAUSE, DEATH, WIN, CUTSCENE_START, CUTSCENE_END, DEATHANIMATION
};

#define BLUE 1
#define ORANGE 0

struct KDTree;
class BVH;

class GameManager {
public:
    GameManager(GLFWwindow *window, const std::string &resourceDir);
    virtual ~GameManager();

    void initScene();
    State processInputs();
    void updateGame(double dt);
    void renderGame(int fps);
    static void resize_callback(GLFWwindow *window, int width, int height);

    State getState() const {
        return gameState;
    }

    int getNumObjCollected() const {
        return numObjCollected;
    }

    int getNumObj() const {
        return objects.size();
    }
private:
    void resolveMagneticInteractions();
    void printStringToScreen(float x, float y, const std::string &text, float r, float g, float b);
    float randFloat(float l, float h);
    void calcOrthoProjs(const glm::mat4 &viewMat);
    void importLevel(std::string level);
    void parseLight(std::string level);
    void parseCamera(std::string level);
    void parseObject(std::string level, std::shared_ptr<Material> greyBox,
            std::shared_ptr<Material> magnetSurface,
            std::shared_ptr<Material> spacePart);

    bool toBool(std::string s);
    void drawScene(std::shared_ptr<MatrixStack> P,
            std::shared_ptr<MatrixStack> V,
            bool depthBufferPass);
    void drawShipPart(std::shared_ptr<MatrixStack> P,
            std::shared_ptr<MatrixStack> V,
            bool depthBufferPass);
    void drawMagnetGun(std::shared_ptr<MatrixStack> P,
            std::shared_ptr<MatrixStack> V,
            bool depthBufferPass);


    GLFWwindow *window;
    std::string RESOURCE_DIR;
    State gameState;

    std::shared_ptr<BulletManager> bullet;
    std::shared_ptr<VfcManager> vfc;
    std::shared_ptr<FmodManager> fmod;
    std::shared_ptr<ShadowManager> shadowManager;
    std::shared_ptr<InputManager> inputManager;
    std::shared_ptr<Bloom> bloom;

    std::shared_ptr<GuiManager> gui;
    std::shared_ptr<ParticleManager> psystem;

    std::shared_ptr<KDTree> kdtree;
    std::shared_ptr<BVH> bvh;

    std::vector<std::shared_ptr<GameObject>> objects;

    std::shared_ptr<GameObject> magnetGun;
    std::shared_ptr<GameObject> magnetBeamOrange;
    std::shared_ptr<GameObject> magnetBeamBlue;
    std::shared_ptr<GameObject> spaceship;
    std::shared_ptr<GameObject> asteroid;
    std::shared_ptr<GameObject> asteroid2;

    std::vector<std::shared_ptr<GameObject>> deathObjects;

    double objIntervalCounter;
    int numObjCollected;
    int gameWon;
    int level;
    bool drawBeam;
    int colorBeam;

    bool playStartSound;
    bool playGunGet;
    bool drawShipParts;
    bool fadeFromBlack;
    float fromBlackAlpha;
    bool fadeToBlack;
    float toBlackAlpha;
    int cutsceneTime;
    bool playBoom;
    bool endFade;

    float shadowOrthoInfo[NUM_SHADOW_CASCADES][6];
    float cascadeEnd[4];
    std::shared_ptr<Camera> camera;

    std::shared_ptr<Program> program, shipPartProgram, skyscraperProgram,
    depthProg, debugProg, asteroidProgram, gaussianProg, bloomProg;
    std::shared_ptr<Texture> shipPartColorTexture, shipPartSpecularTexture,
    skyscraperColorTexture, skyscraperSpecularTexture;
    std::map<std::string, std::shared_ptr<Shape>> shapes;

    std::shared_ptr<SpaceShipPart> spaceShipPart;

    std::shared_ptr<SpaceShipPart> spaceShipPart1;
    std::shared_ptr<SpaceShipPart> spaceShipPart2;
    std::shared_ptr<SpaceShipPart> spaceShipPart3;

    std::shared_ptr<Skybox> skybox;
    std::shared_ptr<Skybox> spacebox;

    glm::vec4 lightPos;
    float lightIntensity;
    glm::mat4 LSpace;
    
    double pausedXMouse, pausedYMouse;
};

#endif /* GameManager_hpp */
