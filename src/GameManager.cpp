//
//  GameManager.cpp
//  Lab1
//
//  Created by Cody Rhoads on 1/17/17.
//
//

#include <iostream>
#include <math.h>
#include <time.h>


#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "GameManager.hpp"

#include "AABoundingBox.hpp"
#include "BoundingSphere.hpp"

#include "InputManager.hpp"
#include "Camera.hpp"
#include "GLSL.h"
#include "MatrixStack.h"
#include "Program.h"
#include "Shape.h"
#include "Material.hpp"
#include "GameObject.hpp"
#include "Texture.h"
#include "Keyboard.hpp"
#include "Mouse.hpp"
#include "BulletManager.hpp"
#include "Cuboid.hpp"
#include "RayTrace.hpp"
#include "FmodManager.hpp"
#include "VfcManager.hpp"
#include "SpaceShipPart.hpp"
#include "Skybox.hpp"
#include "ShadowManager.hpp"
#include "Bloom.hpp"
#include <fstream>

#include "GuiManager.hpp"
#include "ParticleManager.hpp"

#include "KDTree.hpp"
#include "BVH.hpp"

#define SHADOW_DEBUG 0
#define BLOOM_DEBUG 0
#define MAGNET_RANGE 13.0f
#define MAGNET_STRENGTH 7.0f
#define CUBE_HALF_EXTENTS vec3(0.5f, 0.5f, 0.5f)

using namespace std;
using namespace glm;

GameManager::GameManager(GLFWwindow *window, const string &resourceDir) :
window(window),
RESOURCE_DIR(resourceDir),
gameState(MENU),
level(0),
drawBeam(false),
colorBeam(ORANGE),
drawShipParts(false),
fadeToBlack(false),
toBlackAlpha(0.0f),
fadeFromBlack(true),
cutsceneTime(0),
playStartSound(false),
fromBlackAlpha(1.0f),
playGunGet(false),
playBoom(false),
endFade(false) {
    objIntervalCounter = 0.0f;
    numObjCollected = 0;
    gameWon = false;
    camera = make_shared<Camera>();

    // Set vsync.
    glfwSwapInterval(1);
    // Set keyboard callback.
    glfwSetKeyCallback(window, Keyboard::key_callback);
    // Sets cursor movement to unlimited.
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, Mouse::cursor_position_callback);
    // Set the mouse button callback.
    glfwSetMouseButtonCallback(window, Mouse::mouse_button_callback);
    // Set the window resize call back.
    glfwSetFramebufferSizeCallback(window, resize_callback);
    // Set up the manager for keyboard and mouse input.
    inputManager = make_shared<InputManager>(camera);
    //init vfc
    vfc = make_shared<VfcManager>();
    fmod = make_shared<FmodManager>(RESOURCE_DIR);
    gui = make_shared<GuiManager>(RESOURCE_DIR);
    psystem = make_shared<ParticleManager>(RESOURCE_DIR);

    shadowManager = make_shared<ShadowManager>();
    shadowManager->init();

    GLSL::checkError();
    bloom = make_shared<Bloom>();
    bloom->init();
    GLSL::checkError();

    // Initialize the scene.
    initScene();
}

GameManager::~GameManager() {

}

void GameManager::initScene() {
    // Initialize time.
    glfwSetTime(0.0);

    // Set background color.
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    // Enable z-buffer test.
    glEnable(GL_DEPTH_TEST);

    //
    // Objects
    //
    program = make_shared<Program>();
    program->setShaderNames(RESOURCE_DIR + "objectVert.glsl", RESOURCE_DIR + "objectFrag.glsl");
    program->setVerbose(false);
    program->init();
    program->addAttribute("aPos");
    program->addAttribute("aNor");
    program->addUniform("M");
    program->addUniform("V");
    program->addUniform("P");
    program->addUniform("lightPos");
    program->addUniform("lightIntensity");
    program->addUniform("ka");
    program->addUniform("kd");
    program->addUniform("ks");
    program->addUniform("s");
    program->addUniform("viewPos");
    program->addUniform("shadowDepth");
    program->addUniform("LS");

    //
    // Skyscrapers
    //
    skyscraperProgram = make_shared<Program>();
    skyscraperProgram->setShaderNames(RESOURCE_DIR + "skyscraperVert.glsl", RESOURCE_DIR + "skyscraperFrag.glsl");
    skyscraperProgram->setVerbose(false);
    skyscraperProgram->init();
    skyscraperProgram->addAttribute("aPos");
    skyscraperProgram->addAttribute("aNor");
    skyscraperProgram->addAttribute("aTex");
    skyscraperProgram->addAttribute("aTangent");
    skyscraperProgram->addAttribute("aBitangent");
    skyscraperProgram->addUniform("M");
    skyscraperProgram->addUniform("V");
    skyscraperProgram->addUniform("P");
    skyscraperProgram->addUniform("diffuseTex");
    skyscraperProgram->addUniform("specularTex");
    skyscraperProgram->addUniform("normalTex");
    skyscraperProgram->addUniform("lightPos");
    skyscraperProgram->addUniform("viewPos");
    skyscraperProgram->addUniform("lightIntensity");
    skyscraperProgram->addUniform("scalingFactor");
    skyscraperProgram->addUniform("shadowDepth");
    skyscraperProgram->addUniform("LS");

    //
    // Asteroids
    //
    asteroidProgram = make_shared<Program>();
    asteroidProgram->setShaderNames(RESOURCE_DIR + "asteroidVert.glsl", RESOURCE_DIR + "asteroidFrag.glsl");
    asteroidProgram->setVerbose(false);
    asteroidProgram->init();
    asteroidProgram->addAttribute("aPos");
    asteroidProgram->addAttribute("aNor");
    asteroidProgram->addAttribute("aTex");
    asteroidProgram->addAttribute("aTangent");
    asteroidProgram->addAttribute("aBitangent");
    asteroidProgram->addUniform("M");
    asteroidProgram->addUniform("V");
    asteroidProgram->addUniform("P");
    asteroidProgram->addUniform("diffuseTex");
    asteroidProgram->addUniform("specularTex");
    asteroidProgram->addUniform("normalTex");
    asteroidProgram->addUniform("lightPos");
    asteroidProgram->addUniform("viewPos");

    //
    // Ship Parts
    //
    shipPartProgram = make_shared<Program>();
    shipPartProgram->setShaderNames(RESOURCE_DIR + "shipPartVert.glsl",
            RESOURCE_DIR + "shipPartFrag.glsl");
    shipPartProgram->setVerbose(false);
    shipPartProgram->init();

    shipPartProgram->addAttribute("aPos");
    shipPartProgram->addAttribute("aNor");
    shipPartProgram->addAttribute("aTex");
    shipPartProgram->addUniform("M");
    shipPartProgram->addUniform("V");
    shipPartProgram->addUniform("P");
    shipPartProgram->addUniform("diffuseTex");
    shipPartProgram->addUniform("specularTex");
    shipPartProgram->addUniform("lightPos");
    shipPartProgram->addUniform("viewPos");
    shipPartProgram->addUniform("shadowDepth");
    shipPartProgram->addUniform("LS");

    shipPartColorTexture = make_shared<Texture>();
    shipPartColorTexture->setFilename(RESOURCE_DIR + "shipPartColor.jpg");
    shipPartColorTexture->init();
    shipPartColorTexture->setUnit(0);
    shipPartColorTexture->setWrapModes(GL_REPEAT, GL_REPEAT);
    shipPartSpecularTexture = make_shared<Texture>();
    shipPartSpecularTexture->setFilename(RESOURCE_DIR + "shipPartSpecular.jpg");
    shipPartSpecularTexture->init();
    shipPartSpecularTexture->setUnit(1);
    shipPartSpecularTexture->setWrapModes(GL_REPEAT, GL_REPEAT);

    //
    // Stuff for bloom
    //
    gaussianProg = make_shared<Program>();
    gaussianProg->setShaderNames(RESOURCE_DIR + "gaussianVert.glsl",
            RESOURCE_DIR + "gaussianFrag.glsl");
    gaussianProg->setVerbose(false);
    gaussianProg->init();
    gaussianProg->addAttribute("aPos");
    gaussianProg->addUniform("image");
    gaussianProg->addUniform("horizontal");

    bloomProg = make_shared<Program>();
    bloomProg->setShaderNames(RESOURCE_DIR + "bloomVert.glsl",
            RESOURCE_DIR + "bloomFrag.glsl");
    bloomProg->setVerbose(false);
    bloomProg->init();
    bloomProg->addAttribute("aPos");
    bloomProg->addUniform("scene");
    bloomProg->addUniform("bloomBlur");

    //
    // Loading obj files
    //
    shared_ptr<Shape> temp = make_shared<Shape>();

    temp->loadMesh(RESOURCE_DIR + "cube.obj", RESOURCE_DIR);
    temp->fitToUnitBox();
    temp->init();
    shapes.insert(make_pair("plainCube", temp));

    temp = make_shared<Shape>();
    temp->loadMesh(RESOURCE_DIR + "texturedSkyscraper.obj", RESOURCE_DIR);
    temp->fitToUnitBox();
    temp->init();
    shapes.insert(make_pair("skyscraper", temp));

    temp = make_shared<Shape>();
    temp->loadMesh(RESOURCE_DIR + "shipPart.obj", RESOURCE_DIR);
    temp->fitToUnitBox();
    temp->init();
    shapes.insert(make_pair("shipPart", temp));

    temp = make_shared<Shape>();
    temp->loadMesh(RESOURCE_DIR + "Magnagun.obj", RESOURCE_DIR);
    temp->fitToUnitBox();
    temp->init();
    shapes.insert(make_pair("magnetGun", temp));

    temp = make_shared<Shape>();
    temp->loadMesh(RESOURCE_DIR + "cylinder.obj", RESOURCE_DIR);
    temp->fitToUnitBox();
    temp->init();
    shapes.insert(make_pair("cylinder", temp));

    temp = make_shared<Shape>();
    temp->loadMesh(RESOURCE_DIR + "sphere.obj", RESOURCE_DIR);
    temp->fitToUnitBox();
    temp->init();
    shapes.insert(make_pair("sphere", temp));

    temp = make_shared<Shape>();
    temp->loadMesh(RESOURCE_DIR + "asteroid.obj", RESOURCE_DIR);
    temp->fitToUnitBox();
    temp->init();
    shapes.insert(make_pair("asteroid", temp));

    /* Shadow stuff */
    // Initialize the GLSL programs
    depthProg = make_shared<Program>();
    depthProg->setVerbose(false);
    depthProg->setShaderNames(RESOURCE_DIR + "depthVert.glsl", RESOURCE_DIR + "depthFrag.glsl");
    depthProg->init();

    depthProg->addUniform("LS");
    depthProg->addUniform("M");
    depthProg->addAttribute("aPos");
    //un-needed, but easier then modifying shape
    depthProg->addAttribute("aNor");
    depthProg->addAttribute("aTex");

    if (SHADOW_DEBUG) {
        debugProg = make_shared<Program>();
        debugProg->setVerbose(false);
        debugProg->setShaderNames(RESOURCE_DIR + "debugVert.glsl", RESOURCE_DIR + "depthDebugFrag.glsl");
        debugProg->init();

        debugProg->addUniform("texBuf");
        debugProg->addAttribute("aPos");
    } else if (BLOOM_DEBUG) {
        debugProg = make_shared<Program>();
        debugProg->setVerbose(false);
        debugProg->setShaderNames(RESOURCE_DIR + "debugVert.glsl", RESOURCE_DIR + "debugFrag.glsl");
        debugProg->init();

        debugProg->addUniform("texBuf");
        debugProg->addAttribute("aPos");
    }

    temp = make_shared<Shape>();
    temp->loadMesh(RESOURCE_DIR + "temp/untitled.obj", RESOURCE_DIR + "temp/");
    temp->fitToUnitBox();
    temp->init();
    shapes.insert(make_pair("spaceship", temp));

    //
    // Make Skybox
    //
    skybox = make_shared<Skybox>(RESOURCE_DIR, shapes["sphere"], 1);
    spacebox = make_shared<Skybox>(RESOURCE_DIR, shapes["sphere"], 0);

    lightPos = vec4(4.0f, 15.0f, 4.0f, 0.0f);
    lightIntensity = 0.6f;

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    GLSL::checkError(GET_FILE_LINE);

    shared_ptr<Material> material = make_shared<Material>(vec3(0.2f, 0.2f, 0.2f), vec3(0.0f, 0.5f, 0.5f), vec3(1.0f, 0.9f, 0.8f), 200.0f);
    magnetGun = make_shared<GameObject>(vec3(0.42, -0.27, -1), vec3(0.62, 0, -0.109), vec3(1, 1, 1), 0, shapes["magnetGun"], nullptr);
    shared_ptr<Material> material2 = make_shared<Material>(vec3(1.0f, 0.65f, 0.0f), vec3(1.0f, 0.65f, 0.0f), vec3(1.0f, 0.65f, 0.0f), 200.0f);
    magnetBeamOrange = make_shared<GameObject>(vec3(0.18, -0.15, -3), vec3(0.1, 0, -0.01), vec3(0.2, 0.2, 3.5), 0, shapes["cylinder"], material2);
    magnetBeamOrange->setYRot(-0.08f);
    shared_ptr<Material> material3 = make_shared<Material>(vec3(0.5f, 1.0f, 1.0f), vec3(0.5f, 1.0f, 1.0f), vec3(0.5f, 0.5f, 1.0f), 200.0f);
    magnetBeamBlue = make_shared<GameObject>(vec3(0.18, -0.15, -3), vec3(0.1, 0, -0.01), vec3(0.2, 0.2, 3.5), 0, shapes["cylinder"], material3);
    magnetBeamBlue->setYRot(-0.08f);

    spaceship = make_shared<GameObject>(vec3(6.06999, 2.4, 3.7), vec3(1, 0, 0), vec3(5, 5, 5), 0, shapes["spaceship"], material3);
    asteroid = make_shared<GameObject>(vec3(6.06999, 6.4, -1.7), vec3(1, 0, 0), vec3(5, 5, 5), 0, shapes["asteroid"], nullptr);
    asteroid2 = make_shared<GameObject>(vec3(6.06999, -26.4, -1.7), vec3(1, 0, 0), vec3(5, 5, 5), 0, shapes["asteroid"], nullptr);
    asteroid->setScale(vec3(10, 10, 10));
    asteroid2->setScale(vec3(10, 10, 10));

    shared_ptr<Material> spacePart = make_shared<Material>(vec3(0.2f, 0.2f, 0.2f),
            vec3(1.0f, 1.0f, 0.0f),
            vec3(1.0f, 0.9f, 0.8f),
            200.0f);
    spaceShipPart1 = make_shared<SpaceShipPart>(vec3(6, 5.4, 4.0), vec3(0, 0, 0),
            CUBE_HALF_EXTENTS, vec3(1, 1, 1),
            shapes["shipPart"], spacePart);
    spaceShipPart2 = make_shared<SpaceShipPart>(vec3(6, 5.4, 5.0), vec3(0, 0, 0),
            CUBE_HALF_EXTENTS, vec3(1, 1, 1),
            shapes["shipPart"], spacePart);
    spaceShipPart3 = make_shared<SpaceShipPart>(vec3(5, 5.4, 4.3), vec3(0, 0, 0),
            CUBE_HALF_EXTENTS, vec3(1, 1, 1),
            shapes["shipPart"], spacePart);
}

bool GameManager::toBool(string s) {
    return s != "0";
}

void GameManager::parseLight(string objectString) {
    char *str = new char[objectString.length() + 1];
    strcpy(str, objectString.c_str());

    vector<string> elems;
    char *tok = strtok(str, ",");
    while (tok != NULL) {
        elems.push_back(tok);
        tok = strtok(NULL, ",");
    }
    //split(objectString, ',', back_inserter(elems));
    string::size_type sz;
    vec3 pos = vec3(stof(elems[0], &sz), stof(elems[1]), stof(elems[2]));
    //    vec3 scale = vec3(stof(elems[3]), stof(elems[4]), stof(elems[5]));
    //    vec3 rot = vec3(stof(elems[6]), stof(elems[7]), stof(elems[8]));
    //    bool magnetic = toBool(elems[9]);
    //    bool deadly = toBool(elems[10]);
    //    bool playerSpawn = toBool(elems[11]);
    //    bool collectable = toBool(elems[12]);
    bool light = toBool(elems[13]);

    if (light) {
        lightPos = vec4(pos, 0.0);
    }
}

void GameManager::parseCamera(string objectString) {
    char *str = new char[objectString.length() + 1];
    strcpy(str, objectString.c_str());

    vector<string> elems;
    char *tok = strtok(str, ",");
    while (tok != NULL) {
        elems.push_back(tok);
        tok = strtok(NULL, ",");
    }
    //split(objectString, ',', back_inserter(elems));
    string::size_type sz;
    vec3 pos = vec3(stof(elems[0], &sz), stof(elems[1]), stof(elems[2]));
    //vec3 scale = vec3(stof(elems[3]), stof(elems[4]), stof(elems[5]));
    vec3 scale = vec3(0.5f, 1.0f, 0.5f);
    //    vec3 rot = vec3(stof(elems[6]), stof(elems[7]), stof(elems[8]));
    //    bool magnetic = toBool(elems[9]);
    //    bool deadly = toBool(elems[10]);
    //    bool playerSpawn = toBool(elems[11]);
    //    bool collectable = toBool(elems[12]);
    //    bool light = toBool(elems[13]);

    //    cerr << "new camera" << endl;
    //    cerr << pos.x << " " << pos.y << endl;
    //    cerr << scale.x << " " << scale.y << endl;

    camera = make_shared<Camera>(pos, window);
    inputManager->setCamera(camera);
    bullet->createBox("cam", pos, CUBE_HALF_EXTENTS, scale, 1);
}

void GameManager::parseObject(string objectString, shared_ptr<Material> greyBox,
        shared_ptr<Material> magnetSurface,
        shared_ptr<Material> spacePart) {
    static int name = 0;

    char *str = new char[objectString.length() + 1];
    strcpy(str, objectString.c_str());

    vector<string> elems;
    char *tok = strtok(str, ",");
    while (tok != NULL) {
        elems.push_back(tok);
        tok = strtok(NULL, ",");
    }
    //split(objectString, ',', back_inserter(elems));
    string::size_type sz;
    vec3 pos = vec3(stof(elems[0], &sz), stof(elems[1]), stof(elems[2]));
    vec3 scale = vec3(stof(elems[3]), stof(elems[4]), stof(elems[5]));
    //    vec3 rot = vec3(stof(elems[6]), stof(elems[7]), stof(elems[8]));
    bool magnetic = toBool(elems[9]);
    bool deadly = toBool(elems[10]);
    //    bool playerSpawn = toBool(elems[11]);
    bool collectable = toBool(elems[12]);
    //    bool light = toBool(elems[13]);
    //    cerr << "new obj" << endl;
    //    cerr << pos.x << " " << pos.y << endl;
    //    cerr << scale.x << " " << scale.y << endl;

    /*TODO: MAGNETIC AND DEADLY*/
    if (magnetic) {
        shared_ptr<Cuboid> magnet = make_shared<Cuboid>(pos, vec3(0, 0, 0),
                CUBE_HALF_EXTENTS,
                scale, 0, shapes["plainCube"],
                magnetSurface, true);
        objects.push_back(magnet);
        bullet->createMagneticBox(to_string(name++), pos, CUBE_HALF_EXTENTS, scale, 0);
    } else if (deadly) {
        shared_ptr<Cuboid> dobj1 = make_shared<Cuboid>(pos, vec3(0, 0, 0),
                CUBE_HALF_EXTENTS,
                scale, 0, shapes["plainCube"],
                greyBox, false);
        deathObjects.push_back(dobj1);

        //cout << "death box" << endl;
    } else if (collectable) {
        spaceShipPart = make_shared<SpaceShipPart>(pos, vec3(0, 0, 0),
                CUBE_HALF_EXTENTS, scale,
                shapes["shipPart"], spacePart);
        bullet->createBox(to_string(name++), pos, CUBE_HALF_EXTENTS, scale, 0);
    } else {
        shared_ptr<Cuboid> groundPlane = make_shared<Cuboid>(pos, vec3(0, 0, 0),
                CUBE_HALF_EXTENTS,
                scale, 0, shapes["skyscraper"],
                nullptr, false);
        objects.push_back(groundPlane);
        bullet->createBox(to_string(name++), pos, CUBE_HALF_EXTENTS, scale, 0);
    }
}

void GameManager::importLevel(string level) {
    string line;
    ifstream file;
    kdtree = nullptr;
    bvh = nullptr;
    objects.clear();
    deathObjects.clear();

    fadeToBlack = false;
    toBlackAlpha = 0.0f;

    fadeFromBlack = true;
    fromBlackAlpha = 1.0f;
    bullet = make_shared<BulletManager>();
    cutsceneTime = 0;
    playStartSound = false;
    playGunGet = false;
    drawShipParts = false;
    playBoom = false;
    endFade = false;

    if (level == "0")
        spaceship->setPosition(vec3(6.06999, 2.4, 3.7));

    if (level == "1")
        bullet->createMagneticBox(to_string(-1), spaceship->getPosition(), CUBE_HALF_EXTENTS, vec3(2, 2, 2), 0);

    file.open(RESOURCE_DIR + "levels/" + level);

    shared_ptr<Material> greyBox = make_shared<Material>(vec3(0.9f, 0.9f, 0.9f),
            vec3(1.0f, 1.0f, 1.0f),
            vec3(0.0f, 0.0f, 0.0f),
            200.0f);
    shared_ptr<Material> magnetSurface = make_shared<Material>(vec3(0.5f, 0.2f, 0.2f),
            vec3(1.0f, 0.0f, 0.0f),
            vec3(1.0f, 0.9f, 0.8f),
            200.0f);
    shared_ptr<Material> spacePart = make_shared<Material>(vec3(0.2f, 0.2f, 0.2f),
            vec3(1.0f, 1.0f, 0.0f),
            vec3(1.0f, 0.9f, 0.8f),
            200.0f);

    if (file.is_open()) {
        if (getline(file, line)) {
            parseLight(line);
        }
        if (getline(file, line)) {
            /*get camera position*/
            parseCamera(line);
        }
        if (getline(file, line)) {
            /*get spaceship positions*/
            parseObject(line, greyBox, magnetSurface, spacePart);
        }
        while (getline(file, line)) {
            /*objects here*/
            parseObject(line, greyBox, magnetSurface, spacePart);
        }
        cout << "Level '" << level << "' successfully imported." << endl;
        file.close();
    } else {
        cout << "Unable to open level '" << level << "'" << endl;
    }

    bvh = make_shared<BVH>(objects);
    //bvh->printTree();

    Mouse::resetMouse(window, 0, 0);
}

State GameManager::processInputs() {
    if (gameState == GAME) {
        if (!fmod->isPlaying("game"))
            fmod->playSound("game", true, 0.9f);
        gameState = inputManager->processGameInputs(bullet, fmod);
        if (gameState == PAUSE) {
            pausedXMouse = Mouse::getMouseX();
            pausedYMouse = Mouse::getMouseY();
        }

        if (Keyboard::isPressed(']')) {
            //fmod->playSound("win",false);
            fmod->playSound("cheater",false);
            gui->resetWin();
            gameState = WIN;
        }
    } else if (gameState == PAUSE) {
        gameState = inputManager->processPauseInputs(gui, fmod);
        if (gameState == GAME) {
            Mouse::resetMouse(window, pausedXMouse, pausedYMouse);
        }
    } else if (gameState == MENU) {
        gameState = inputManager->processMenuInputs(gui, fmod);
        if (fmod->isPlaying("game"))
            fmod->stopSound("game");
        if (!fmod->isPlaying("menu"))
            fmod->playSound("menu", true);
        if (gameState == GAME) {
            fmod->stopSound("menu");

            importLevel(to_string(level));
        }
        if (gameState == CUTSCENE_START || gameState == CUTSCENE_END) {
            spaceShipPart1->setPosition(vec3(6, 5.4, 4.0));
            spaceShipPart2->setPosition(vec3(6, 5.4, 5.0));
            spaceShipPart3->setPosition(vec3(5, 5.4, 4.3));

            fmod->stopSound("menu");
            fmod->playSound("flying", true, 0.3);
            importLevel(to_string(level));


            camera->setYaw(-7.85);
            camera->setPitch(-0.14);

        }
    } else if (gameState == DEATH) {
        gameState = inputManager->processDeathInputs(gui, fmod);
        if (gameState == GAME) {
            importLevel(to_string(level));
        }
    } else if (gameState == WIN) {
        gameState = inputManager->processWinInputs(gui, fmod);
        if (gameState == GAME) {

            level++;
            if (level == NUMLEVELS) {
                gameState = CUTSCENE_END;
                spaceship->setPosition(vec3(1.119955, -0.490005, 3.55));

            }

            importLevel(to_string(level));
        } else if (gameState == MENU) {
            fmod->stopSound("menu");
        }
    } else if (gameState == CUTSCENE_START || gameState == CUTSCENE_END) {
        gameState = inputManager->processCutsceneInputs(bullet, fmod, spaceship, gameState, gui);

        if (gameState == GAME) {
            level++;

            fmod->stopSound("flying");
            if (fmod->isPlaying("gps"))
                fmod->stopSound("gps");
            if (fmod->isPlaying("error"))
                fmod->stopSound("error");
            if (fmod->isPlaying("crash"))
                fmod->stopSound("crash");
            vec3 old = spaceship->getPosition();
            old.y += 2.0f;
            spaceship->setPosition(old);
            importLevel(to_string(level));
            bullet->createMagneticBox(to_string(-1), spaceship->getPosition(), CUBE_HALF_EXTENTS, vec3(2, 2, 2), 0);
        } else if (gameState == MENU) {
            if (fmod->isPlaying("flying"))
                fmod->stopSound("flying");
            if (fmod->isPlaying("gps"))
                fmod->stopSound("gps");
            if (fmod->isPlaying("error"))
                fmod->stopSound("error");
            if (fmod->isPlaying("crash"))
                fmod->stopSound("crash");
            if (fmod->isPlaying("boom"))
                fmod->stopSound("boom");
            level = 0;
        }

    }


    if ((gameState == GAME)) {
        // Set cursor position callback.
        glfwSetCursorPosCallback(window, Mouse::cursor_position_callback);
    } else {
        glfwSetCursorPosCallback(window, NULL);
    }


    return gameState;
}

void GameManager::updateGame(double dt) {
    //bullet->rayTrace(camera->getPosition(), camera->getPosition() + (camera->getDirection() * MAGNET_RANGE));

    if (gameState == MENU) {
        gui->update();
    } else {
        gui->resetMenu();

        if (gameState != CUTSCENE_START && gameState != CUTSCENE_END) {
            /*scripted stuff for level 1*/
            if (level == 1) {

                if (!playStartSound && 0 == 1) {
                    playStartSound = true;
                    fmod->playSound("start", false);
                }

                if (!playGunGet && !fmod->isPlaying("start") && 0 == 1) {
                    fmod->playSound("gunget", false);
                    playGunGet = true;
                }
            }

            if (!fmod->isPlaying("start") && gameState != MENU) {
                resolveMagneticInteractions();
            }

            spaceShipPart->update(dt);

            //step the bullet, update test obj
            bullet->step(dt);

            camera->setPosition(bullet->getBulletObjectState("cam"));

            psystem->update(dt, camera->getPosition());

            if (camera->checkForCollision(spaceShipPart)) {
                spaceShipPart->startWin();
                if (!fmod->isPlaying("collecting")) {
                    fmod->playSound("collecting", true, 1.5);
                }
            }
            if (spaceShipPart->doneWinning()) {
                kdtree = nullptr;
                bvh = nullptr;
                objects.clear();
                deathObjects.clear();
                if (fmod->isPlaying("collecting")) {
                    fmod->stopSound("collecting");
                }
                fmod->playSound("win", false, 0.7f);
                gui->resetWin();
                gameState = WIN;
            }

            /*check for collision with death objects*/
            for (unsigned int i = 0; i < deathObjects.size(); i++) {
                if (camera->checkForCollision(deathObjects.at(i))) {
                    fmod->playSound("death", false);
                    if (fmod->isPlaying("start"))
                        fmod->stopSound("start");
                    if (fmod->isPlaying("collecting")) {
                        fmod->stopSound("collecting");
                    }
                    gameState = DEATHANIMATION;
                }
            }

        }/* cutscene stuff*/
        else if (level == 0) {
            static vec3 orig = camera->getPosition();
            cutsceneTime++;

            vec3 old1 = asteroid->getPosition();
            vec3 old2 = asteroid2->getPosition();

            old1.z += 1.5f;
            old2.z += 1.f;

            static bool a1 = false;
            static bool a2 = false;
            if (old1.z >= -60.0f && !a1) {
                old2.z = -200.0f;
                old2.y = randFloat(6.0f, 8.0f);
                old2.x = randFloat(-35.0f, 25.0f);
                a1 = true;
                a2 = false;
            }
            else if (old2.z >= -60.0f && !a2) {
                old1.z = -200.0f;
                old1.y = randFloat(6.0f, 8.0f);
                old1.x = randFloat(-35.0f, 25.0f);
                a1 = false;
                a2 = true;
            }


            asteroid->setPosition(old1);
            asteroid2->setPosition(old2);

            //cout << asteroid->getPosition().x << " " << asteroid->getPosition().y << endl;

            if (cutsceneTime % 50 > 25) {

                //cout << "here" << endl;
            }
            /*
            if (cutsceneTime == 400) {
                if (!fmod->isPlaying("gps"))
                    fmod->playSound("gps", false);

            }
             */
            if (cutsceneTime == 400) {
                if (!fmod->isPlaying("error"))
                    fmod->playSound("error", false);
            }

            if (cutsceneTime > 800) {
                drawShipParts = true;
                float r1 = -0.001 + static_cast<float> (rand()) / (static_cast<float> (RAND_MAX / ((0.001)-(-0.001))));
                float r2 = -0.001 + static_cast<float> (rand()) / (static_cast<float> (RAND_MAX / ((0.001)-(-0.001))));
                float r3 = -0.001 + static_cast<float> (rand()) / (static_cast<float> (RAND_MAX / ((0.001)-(-0.001))));

                vec3 old = camera->getPosition();
                old.x += r1;
                old.y += r2;
                old.z += r3;

                if (abs(old.x - orig.x) >= 0.08 || abs(old.y - orig.y) >= 0.08 || abs(old.z - orig.z) >= 0.08)
                    camera->setPosition(orig);
                else
                    camera->setPosition(old);
            }
            if (cutsceneTime > 1280) {
                float r1 = -0.02 + static_cast<float> (rand()) / (static_cast<float> (RAND_MAX / ((0.02)-(-0.02))));
                float r2 = -0.02 + static_cast<float> (rand()) / (static_cast<float> (RAND_MAX / ((0.02)-(-0.02))));
                float r3 = -0.02 + static_cast<float> (rand()) / (static_cast<float> (RAND_MAX / ((0.02)-(-0.02))));

                vec3 old = camera->getPosition();
                old.x += r1;
                old.y += r2;
                old.z += r3;

                if (abs(old.x - orig.x) >= 0.08 || abs(old.y - orig.y) >= 0.08 || abs(old.z - orig.z) >= 0.08)
                    camera->setPosition(orig);
                else
                    camera->setPosition(old);

                fadeToBlack = true;
            }

            if (drawShipParts) {
                vec3 old1 = spaceShipPart1->getPosition();
                vec3 old2 = spaceShipPart2->getPosition();
                vec3 old3 = spaceShipPart3->getPosition();
                float z = 0.09f;
                old1.z -= z;
                old2.z -= z;
                old3.z -= z;

                float y = 0.04f;
                old1.y -= y;
                old2.y -= y;
                old3.y -= y;
                spaceShipPart1->setPosition(old1);
                spaceShipPart2->setPosition(old2);
                spaceShipPart3->setPosition(old3);
            }

            if (cutsceneTime == 1500) {

                fmod->stopSound("flying");
                vec3 old = spaceship->getPosition();
                old.y += 2.0f;
                spaceship->setPosition(old);
                level++;
                importLevel(to_string(level));
                bullet->createMagneticBox(to_string(-1), spaceship->getPosition(), CUBE_HALF_EXTENTS, vec3(2, 2, 2), 0);
                gameState = GAME;
            }


        }//ending cutscene
        else {
            cutsceneTime++;
            vec3 old = spaceship->getPosition();
            old.z -= 0.01f;

            if (!playBoom) {
                if (!fmod->isPlaying("flying"))
                    fmod->playSound("flying", true);
            }

            if (cutsceneTime == 500) {
                if (fmod->isPlaying("flying"))
                    fmod->stopSound("flying");
                if (!playBoom) {
                    playBoom = true;
                    fmod->playSound("boom", false);
                }


                fadeToBlack = true;
            }
            if (cutsceneTime >= 500)
                old.z -= 1.0f;
            spaceship->setPosition(old);

        }
    }
}

void GameManager::drawScene(shared_ptr<MatrixStack> P, shared_ptr<MatrixStack> V,
        bool depthBufferPass) {
    shared_ptr<Program> shaderMagnet, shaderBuilding;
    if (depthBufferPass) {
        shaderMagnet = depthProg;
        shaderBuilding = depthProg;
    } else {
        shaderMagnet = program;
        shaderBuilding = skyscraperProgram;
    }

    GLSL::checkError();
    vfc->extractVFPlanes(P->topMatrix(), V->topMatrix());
    for (unsigned int i = 0; i < objects.size(); i++) {
        if (objects.at(i)->isCuboid()) {
            std::shared_ptr<Cuboid> cub = dynamic_pointer_cast<Cuboid>(objects.at(i));
            std::vector<vec3> *temp = cub->getAabbMinsMaxs();
            GLSL::checkError();
            if (!vfc->viewFrustCull(temp) || depthBufferPass) {
                if (cub->isMagnetic()) {
                    shaderMagnet->bind();
                    shadowManager->setUnit(3);
                    shadowManager->bind(shaderMagnet->getUniform("shadowDepth"));
                    glUniformMatrix4fv(shaderMagnet->getUniform("LS"), 1, GL_FALSE, value_ptr(LSpace));
                    glUniformMatrix4fv(shaderMagnet->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));
                    glUniformMatrix4fv(shaderMagnet->getUniform("V"), 1, GL_FALSE, value_ptr(V->topMatrix()));
                    glUniform3fv(shaderMagnet->getUniform("lightPos"), 1, value_ptr(vec3(lightPos)));
                    glUniform3fv(shaderMagnet->getUniform("viewPos"), 1, value_ptr(camera->getPosition()));
                    glUniform1f(shaderMagnet->getUniform("lightIntensity"), lightIntensity);
                    cub->draw(shaderMagnet);
                    shadowManager->unbind();
                    shaderMagnet->unbind();
                } else {
                    shaderBuilding->bind();
                    shadowManager->setUnit(3);
                    shadowManager->bind(shaderBuilding->getUniform("shadowDepth"));
                    glUniformMatrix4fv(shaderBuilding->getUniform("LS"), 1, GL_FALSE, value_ptr(LSpace));
                    glUniformMatrix4fv(shaderBuilding->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));
                    glUniformMatrix4fv(shaderBuilding->getUniform("V"), 1, GL_FALSE, value_ptr(V->topMatrix()));
                    glUniform3fv(shaderBuilding->getUniform("lightPos"), 1, value_ptr(vec3(lightPos)));
                    glUniform3fv(shaderBuilding->getUniform("viewPos"), 1, value_ptr(camera->getPosition()));
                    glUniform1f(shaderBuilding->getUniform("lightIntensity"), lightIntensity);
                    glUniform3fv(shaderBuilding->getUniform("scalingFactor"), 1, value_ptr(cub->getScale()));
                    GLSL::checkError();
                    cub->draw(shaderBuilding);
                    shadowManager->unbind();
                    shaderBuilding->unbind();
                }
            }

            delete temp;
        }
    }
}

void GameManager::drawShipPart(shared_ptr<MatrixStack> P,
        shared_ptr<MatrixStack> V,
        bool depthBufferPass) {
    std::shared_ptr<Program> shader;
    if (depthBufferPass) {
        shader = depthProg;
    } else {
        shader = shipPartProgram;
    }

    // Draw ship part
    shader->bind();
    shadowManager->setUnit(3);
    shadowManager->bind(shader->getUniform("shadowDepth"));
    glUniformMatrix4fv(shader->getUniform("LS"), 1, GL_FALSE, value_ptr(LSpace));
    shipPartColorTexture->bind(shader->getUniform("diffuseTex"));
    shipPartSpecularTexture->bind(shader->getUniform("specularTex"));
    glUniformMatrix4fv(shader->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));
    glUniformMatrix4fv(shader->getUniform("V"), 1, GL_FALSE, value_ptr(V->topMatrix()));
    glUniform3fv(shader->getUniform("lightPos"), 1, value_ptr(vec3(lightPos)));
    glUniform3fv(shader->getUniform("viewPos"), 1, value_ptr(camera->getPosition()));
    spaceShipPart->draw(shader);
    shipPartSpecularTexture->unbind();
    shipPartColorTexture->unbind();
    shader->unbind();
}

void GameManager::drawMagnetGun(shared_ptr<MatrixStack> P,
        shared_ptr<MatrixStack> V,
        bool depthBufferPass) {
    shared_ptr<Program> shader;
    if (depthBufferPass) {
        shader = depthProg;
    } else {
        shader = program;
    }

    // Render magnet gun
    shader->bind();
    shadowManager->setUnit(3);
    shadowManager->bind(shader->getUniform("shadowDepth"));
    glUniformMatrix4fv(shader->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));
    glUniform3fv(shader->getUniform("lightPos"), 1, value_ptr(vec3(lightPos)));
    glUniform1f(shader->getUniform("lightIntensity"), lightIntensity);
    
    
    
    V->pushMatrix();
    V->loadIdentity();
    glUniformMatrix4fv(shader->getUniform("V"), 1, GL_FALSE, value_ptr(V->topMatrix()));
    if (!depthBufferPass) {
        glClear(GL_DEPTH_BUFFER_BIT);
    }
    GLSL::checkError();
    magnetGun->draw(shader);
    shadowManager->unbind();
    V->popMatrix();

    if (drawBeam) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glUniform1f(shader->getUniform("lightIntensity"), 0.9f);
        if (colorBeam == BLUE) {
            magnetBeamBlue->draw(shader);
            psystem->setColor(BLUE);
        } else {
            magnetBeamOrange->draw(shader);
            psystem->setColor(ORANGE);
        }

        glDisable(GL_BLEND);

        V->pushMatrix();
        V->loadIdentity();
        psystem->draw(V->topMatrix(), P->topMatrix(), 0);
        V->popMatrix();

        glDisable(GL_BLEND);
    }

    glEnable(GL_DEPTH_TEST);
    shader->unbind();
}

mat4 SetOrthoMatrix() {
    mat4 OP = glm::ortho(-30.0, 30.0, -30.0, 30.0, 0.1, 80.0);
    return OP;
}

mat4 SetLightView(vec3 pos, vec3 LA, vec3 up) {
    mat4 Cam = glm::lookAt(pos, LA, up);
    return Cam;
}

void GameManager::renderGame(int fps) {
    int objectsDrawn = 0;

    // Clear framebuffer.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (Keyboard::isPressed(GLFW_KEY_C)) {
        glEnable(GL_CULL_FACE);
    } else {
        glDisable(GL_CULL_FACE);
    }

    if (Keyboard::isPressed(GLFW_KEY_L)) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    // Get current frame buffer size.
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    camera->setAspect((float) width / (float) height);
    gui->setWindowSize(width, height);

    // Matrix stacks
    auto P = make_shared<MatrixStack>();
    auto V = make_shared<MatrixStack>();

    /*if in gamestate menu render menu*/
    if (gameState == MENU) {
        gui->drawMenu();
    }/* else its in pause menu/game*/
    else if (gameState == DEATH) {
        gui->drawDeath();
    } else if (gameState == WIN) {
        gui->drawWin(level);
    } else {
        // Apply camera transforms
        P->pushMatrix();
        camera->applyProjectionMatrix(P);
        V->pushMatrix();
        camera->applyViewMatrix(V);

        if (gameState != CUTSCENE_START && gameState != CUTSCENE_END) {

            /* BEGIN DEPTH MAP */
            shadowManager->bindFramebuffer();
            //set up shadow shader
            depthProg->bind();
            mat4 LO = SetOrthoMatrix();
            mat4 LV = SetLightView(vec3(lightPos), vec3(0), vec3(0, 1, 0));
            LSpace = LO*LV;
            glUniformMatrix4fv(depthProg->getUniform("LS"), 1, GL_FALSE, value_ptr(LSpace));
            drawScene(P, V, true);
            drawShipPart(P, V, true);
            depthProg->unbind();
            shadowManager->unbindFramebuffer();
            /* END DEPTH MAP */

            /* Rendering scene for bloom effects */
            bloom->bindFramebuffer();
            skybox->render(P, V, 0);
            drawScene(P, V, false);
            drawShipPart(P, V, false);


            if (gameState == DEATHANIMATION) {
                toBlackAlpha += 0.04f;
                gui->drawBlack(toBlackAlpha);
                if (toBlackAlpha >= 1.0f) {
                    objects.clear();
                    deathObjects.clear();
                    toBlackAlpha = 0;
                    gameState = DEATH;
                    gui->resetDeath();
                }

            } else if (fadeFromBlack) {
                fromBlackAlpha -= 0.02f;
                gui->drawBlack(fromBlackAlpha);
                if (fromBlackAlpha <= 0.0f) {
                    fadeFromBlack = false;
                    fromBlackAlpha = 1.0f;
                }
            }

            if (level == 1) {
                program->bind();
                //spaceship->setPosition(camera->getPosition());
                glUniformMatrix4fv(program->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));
                glUniformMatrix4fv(program->getUniform("V"), 1, GL_FALSE, value_ptr(V->topMatrix()));
                glUniform3fv(program->getUniform("lightPos"), 1, value_ptr(vec3(lightPos)));
                glUniform1f(program->getUniform("lightIntensity"), lightIntensity);
                spaceship->draw(program);
                program->unbind();
            }
            bloom->unbindFramebuffer();

            /* Blurring the bloom objects */
            gaussianProg->bind();
            bloom->gaussianBlur(gaussianProg->getUniform("horizontal"));
            gaussianProg->unbind();


            GLSL::checkError();

            glViewport(0, 0, width, height);
            glClear(GL_DEPTH_BUFFER_BIT);

            /* Merging the scene and bloom objects */
            bloomProg->bind();
            bloom->applyBloom(bloomProg->getUniform("scene"),
                    bloomProg->getUniform("bloomBlur"));
            bloomProg->unbind();


            GLSL::checkError();

            if (!fmod->isPlaying("start"))
                drawMagnetGun(P, V, false);
            if (gameState != PAUSE && !fmod->isPlaying("start")) {
                gui->drawHUD(camera->isLookingAtMagnet(), Mouse::isLeftMouseButtonPressed(), Mouse::isRightMouseButtonPressed());
            }

            if (gameState == DEATHANIMATION) {
                toBlackAlpha += 0.04f;
                gui->drawBlack(toBlackAlpha);
                if (toBlackAlpha >= 1.0f) {
                    objects.clear();
                    deathObjects.clear();
                    toBlackAlpha = 0;
                    gameState = DEATH;
                    gui->resetDeath();
                }

            } else if (fadeFromBlack) {
                fromBlackAlpha -= 0.02f;
                gui->drawBlack(fromBlackAlpha);
                if (fromBlackAlpha <= 0.0f) {
                    fadeFromBlack = false;
                    fromBlackAlpha = 1.0f;
                }
            }

            if (SHADOW_DEBUG) {
                glClear(GL_DEPTH_BUFFER_BIT);
                glViewport(0, 0, 500, 500);
                debugProg->bind();
                shadowManager->setUnit(0);
                shadowManager->bind(debugProg->getUniform("texBuf"));
                shadowManager->drawDebug();
                debugProg->unbind();
                glViewport(0, 0, width, height);
            } else if (BLOOM_DEBUG) {
                glClear(GL_DEPTH_BUFFER_BIT);
                glViewport(0, 0, 500, 500);
                debugProg->bind();
                bloom->setUnit(0);
                bloom->bindColor(debugProg->getUniform("texBuf"));
                bloom->renderQuad();
                debugProg->unbind();
                glViewport(0, 0, width, height);
            }
        }/*draw cutscene only stuff here*/
        else {
            if (level == 0)
                spacebox->render(P, V, cutsceneTime);

            spacebox->render(P, V, 0);
            program->bind();
            glUniformMatrix4fv(program->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));

            glUniformMatrix4fv(program->getUniform("V"), 1, GL_FALSE, value_ptr(V->topMatrix()));

            glUniform3fv(program->getUniform("lightPos"), 1, value_ptr(vec3(lightPos)));
            glUniform1f(program->getUniform("lightIntensity"), lightIntensity);
            spaceship->draw(program);
            program->unbind();

            if (level == 0) {
                if (drawShipParts) {
                    shipPartProgram->bind();
                    shipPartColorTexture->bind(shipPartProgram->getUniform("diffuseTex"));
                    shipPartSpecularTexture->bind(shipPartProgram->getUniform("specularTex"));
                    glUniformMatrix4fv(shipPartProgram->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));
                    glUniformMatrix4fv(shipPartProgram->getUniform("V"), 1, GL_FALSE, value_ptr(V->topMatrix()));
                    glUniform3fv(shipPartProgram->getUniform("lightPos"), 1, value_ptr(vec3(lightPos)));
                    spaceShipPart1->draw(shipPartProgram);
                    spaceShipPart2->draw(shipPartProgram);
                    spaceShipPart3->draw(shipPartProgram);
                    shipPartSpecularTexture->unbind();
                    shipPartColorTexture->unbind();
                    shipPartProgram->unbind();
                }

                static float angle = 0;
                angle++;

                asteroid->setYRot(angle / 20);
                asteroid2->setYRot(angle / 20);
                asteroidProgram->bind();
                glUniformMatrix4fv(asteroidProgram->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));


                glUniformMatrix4fv(asteroidProgram->getUniform("V"), 1, GL_FALSE, value_ptr(V->topMatrix()));
                glUniform3fv(asteroidProgram->getUniform("lightPos"), 1, value_ptr(vec3(lightPos)));
                glUniform3fv(asteroidProgram->getUniform("viewPos"), 1, value_ptr(camera->getPosition()));
                asteroid->draw(asteroidProgram);
                asteroid2->draw(asteroidProgram);
                asteroidProgram->unbind();

            }


            if (fadeToBlack) {
                if (level == NUMLEVELS) {
                    toBlackAlpha += 0.005f;
                    gui->drawBlack(toBlackAlpha);
                    if (toBlackAlpha >= 1.0f) {
                        endFade = true;
                        toBlackAlpha = 0.0f;
                        fromBlackAlpha = 1.0f;
                        fadeToBlack = false;
                        fadeFromBlack = false;
                        //                        cout << "trigger" << endl;
                        //gameState = MENU;
                        //level = 0;
                    }
                } else {
                    toBlackAlpha += 0.02f;
                    gui->drawBlack(toBlackAlpha);
                    if (toBlackAlpha >= 1.0f) {
                        if (!fmod->isPlaying("crash"))
                            fmod->playSound("crash", false);
                    }
                }
            }
            if (fadeFromBlack) {
                fromBlackAlpha -= 0.005f;
                gui->drawBlack(fromBlackAlpha);
                if (fromBlackAlpha <= 0.0f) {
                    fadeFromBlack = false;
                    fromBlackAlpha = 1.0f;
                }
            }

            if (endFade) {
                if (fromBlackAlpha >= 0.0f) {
                    fromBlackAlpha -= 0.005f;
                    gui->drawEnd(fromBlackAlpha);
                    gui->drawBlack(fromBlackAlpha);
                    //cout << "fade from black " << fromBlackAlpha << endl;

                } else {
                    toBlackAlpha += 0.005f;
                    gui->drawEnd(toBlackAlpha);
                    gui->drawBlack(toBlackAlpha);
                    //                    cout << "fade to black" << endl;
                    if (toBlackAlpha >= 0.9f) {

                        gameState = MENU;
                        level = 0;
                        endFade = false;
                    }
                }
            }


            gui->drawSkip(cutsceneTime);
        }

        V->popMatrix();
        P->popMatrix();

        if (gameState == PAUSE) {
            gui->drawPause(level);
        }
    }
}

// If the window is resized, capture the new size and reset the viewport

void GameManager::resize_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

void GameManager::resolveMagneticInteractions() {
    vec3 startLoc = camera->getPosition();
    vec3 endLoc = startLoc + camera->getDirection() * MAGNET_RANGE;
    float dist;

    //vector<shared_ptr<GameObject>> nearObjs = kdtree->findObjectsIntersectedByRay(startLoc, endLoc);
    //shared_ptr<GameObject> obj = RayTrace::rayTrace(startLoc, endLoc, objects);
    shared_ptr<GameObject> obj = bvh->findClosestHitObject(startLoc, endLoc, &dist);
    drawBeam = false;
    if (obj && obj->isMagnetic()) {
        camera->setLookingAtMagnet(true);
        if (Mouse::isLeftMouseButtonPressed()) {
            if (!fmod->isPlaying("magnet")) {
                fmod->playSound("magnet", false, 0.4);
            }
            vec3 dir = normalize(endLoc - startLoc);
            btVector3 bulletDir = btVector3(dir.x, dir.y, dir.z);
            bullet->getBulletObject("cam")->getRigidBody()->setLinearVelocity(bulletDir * MAGNET_STRENGTH * ((MAGNET_RANGE - dist + 1) / (MAGNET_RANGE / 2.0)));
            drawBeam = true;
            colorBeam = ORANGE;
        } else if (Mouse::isRightMouseButtonPressed()) {
            if (!fmod->isPlaying("magnet")) {
                fmod->playSound("magnet", false, 0.4);
            }
            vec3 dir = normalize(startLoc - endLoc);
            btVector3 bulletDir = btVector3(dir.x, dir.y, dir.z);
            bullet->getBulletObject("cam")->getRigidBody()->setLinearVelocity(bulletDir * MAGNET_STRENGTH * ((MAGNET_RANGE - dist + 1) / (MAGNET_RANGE / 2.0)));
            drawBeam = true;
            colorBeam = BLUE;
        }

    } else {
        camera->setLookingAtMagnet(false);
        drawBeam = false;
        if (Mouse::isLeftMouseButtonPressed() || Mouse::isRightMouseButtonPressed()) {
            if (!fmod->isPlaying("click"))
                fmod->playSound("click", false, 0.5);
        }
    }
}

// Returns a random float between the provided low and high floats.

float GameManager::randFloat(float l, float h) {
    float r = rand() / (float) RAND_MAX;
    return (1.0f - r) * l + r * h;
}
