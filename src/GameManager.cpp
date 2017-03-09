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
#include <fstream>

#include "GuiManager.hpp"
#include "ParticleManager.hpp"

#include "KDTree.hpp"
#include "BVH.hpp"

#define SHADOW 1
#define SHADOW_DEBUG 0
#define MAGNET_RANGE 13.0f
#define MAGNET_STRENGTH 7.0f
#define CUBE_HALF_EXTENTS vec3(0.5f, 0.5f, 0.5f)

using namespace std;
using namespace glm;

// Temp stuff for shadow mapping
shared_ptr<Program> DepthProg;
shared_ptr<Program> DebugProg;
GLuint depthMapFBO;
const GLuint SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
GLuint depthMap;
//geometry for texture render
GLuint quad_VertexArrayID;
GLuint quad_vertexbuffer;
mat4 LSpace;

GameManager::GameManager(GLFWwindow *window, const string &resourceDir) :
window(window),
RESOURCE_DIR(resourceDir),
gameState(MENU),
level(1),
drawBeam(false),
colorBeam(ORANGE) {
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
    // Set cursor position callback.
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

    glGenVertexArrays(1, &quad_VertexArrayID);
    glBindVertexArray(quad_VertexArrayID);
    
    static const GLfloat g_quad_vertex_buffer_data[] = {
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        1.0f,  1.0f, 0.0f,
    };
    
    glGenBuffers(1, &quad_vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data), g_quad_vertex_buffer_data, GL_STATIC_DRAW);
    
    // Initialize the scene.
    initScene();
}

GameManager::~GameManager() {

}

/* set up the FBO for the light's depth */
void initShadow() {
    
    //generate the FBO for the shadow depth
    glGenFramebuffers(1, &depthMapFBO);
    
    //generate the texture
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT,
                 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    //bind with framebuffer's depth buffer
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GameManager::initScene() {
    // Initialize time.
    glfwSetTime(0.0);

    // Set background color.
    glClearColor(0.5f, 1.0f, 1.0f, 1.0f);
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
    
    if (SHADOW) {
        /* Shadow stuff */
        // Initialize the GLSL programs
        DepthProg = make_shared<Program>();
        DepthProg->setVerbose(false);
        DepthProg->setShaderNames(RESOURCE_DIR + "depthVert.glsl", RESOURCE_DIR + "depthFrag.glsl");
        DepthProg->init();
        
        DepthProg->addUniform("LS");
        DepthProg->addUniform("M");
        DepthProg->addAttribute("aPos");
        //un-needed, but easier then modifying shape
        DepthProg->addAttribute("aNor");
        DepthProg->addAttribute("aTex");
        
        DebugProg = make_shared<Program>();
        DebugProg->setVerbose(false);
        DebugProg->setShaderNames(RESOURCE_DIR + "depthDebugVert.glsl", RESOURCE_DIR + "depthDebugFrag.glsl");
        DebugProg->init();
        
        DebugProg->addUniform("texBuf");
        DebugProg->addAttribute("aPos");
        
        initShadow();
    }
    
    //
    // Make Skybox
    //
    skybox = make_shared<Skybox>(RESOURCE_DIR, shapes["sphere"]);

    lightPos = vec4(4.0f, 15.0f, 4.0f, 0.0f);
    lightIntensity = 0.6f;

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    GLSL::checkError(GET_FILE_LINE);

    // Set up the bullet manager and create ground plane and camera.
    //bullet = make_shared<BulletManager>();
    //bullet->createPlane("ground", 0, 0, 0);

    shared_ptr<Material> material = make_shared<Material>(vec3(0.2f, 0.2f, 0.2f), vec3(0.0f, 0.5f, 0.5f), vec3(1.0f, 0.9f, 0.8f), 200.0f);
    magnetGun = make_shared<GameObject>(vec3(0.4, -0.2, -1), vec3(0.6, 0, -0.2), vec3(1, 1, 1), 0, shapes["magnetGun"], nullptr);
    shared_ptr<Material> material2 = make_shared<Material>(vec3(1.0f, 0.65f, 0.0f), vec3(1.0f, 0.65f, 0.0f), vec3(1.0f, 0.65f, 0.0f), 200.0f);
    magnetBeamOrange = make_shared<GameObject>(vec3(0.4, -0.4, -3), vec3(0.1, 0, 0), vec3(0.2, 0.2, 3.5), 0, shapes["cylinder"], material2);
    shared_ptr<Material> material3 = make_shared<Material>(vec3(0.5f, 1.0f, 1.0f), vec3(0.5f, 1.0f, 1.0f), vec3(0.5f, 0.5f, 1.0f), 200.0f);
    magnetBeamBlue = make_shared<GameObject>(vec3(0.4, -0.4, -3), vec3(0.1, 0, 0), vec3(0.2, 0.2, 3.5), 0, shapes["cylinder"], material3);
}

bool GameManager::toBool(string s) {
    return s != "0";
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
    vec3 scale = vec3(stof(elems[3]), stof(elems[4]), stof(elems[5]));
    vec3 rot = vec3(stof(elems[6]), stof(elems[7]), stof(elems[8]));
    bool magnetic = toBool(elems[9]);
    bool deadly = toBool(elems[10]);
    bool playerSpawn = toBool(elems[11]);
    bool collectable = toBool(elems[12]);

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
    vec3 rot = vec3(stof(elems[6]), stof(elems[7]), stof(elems[8]));
    bool magnetic = toBool(elems[9]);
    bool deadly = toBool(elems[10]);
    bool playerSpawn = toBool(elems[11]);
    bool collectable = toBool(elems[12]);
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
    bullet = make_shared<BulletManager>();
    file.open(RESOURCE_DIR + "levels/" + level);

    shared_ptr<Material> greyBox = make_shared<Material>(vec3(0.9f, 0.9f, 0.9f),
            vec3(1.0f, 1.0f, 1.0f),
            vec3(0.0f, 0.0f, 0.0f),
            200.0f);
    shared_ptr<Material> magnetSurface = make_shared<Material>(vec3(0.2f, 0.2f, 0.2f),
            vec3(1.0f, 0.0f, 0.0f),
            vec3(1.0f, 0.9f, 0.8f),
            200.0f);
    shared_ptr<Material> spacePart = make_shared<Material>(vec3(0.2f, 0.2f, 0.2f),
            vec3(1.0f, 1.0f, 0.0f),
            vec3(1.0f, 0.9f, 0.8f),
            200.0f);

    if (file.is_open()) {
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
}

State GameManager::processInputs() {
    if (gameState == GAME) {
        if (!fmod->isPlaying("game"))
            fmod->playSound("game", true);
        gameState = inputManager->processGameInputs(bullet, fmod);
    } else if (gameState == PAUSE) {
        gameState = inputManager->processPauseInputs(gui, fmod);
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
    } else if (gameState == DEATH) {
        gameState = inputManager->processDeathInputs(gui, fmod);
        if (gameState == GAME) {
            importLevel(to_string(level));
        }
    } else if (gameState == WIN) {
        gameState = inputManager->processWinInputs(gui, fmod);
        if (gameState == GAME) {
            level++;
            importLevel(to_string(level));
        } else if (gameState == MENU) {
            fmod->stopSound("menu");
        }
    }

    return gameState;
}

void GameManager::updateGame(double dt) {
    //bullet->rayTrace(camera->getPosition(), camera->getPosition() + (camera->getDirection() * MAGNET_RANGE));
    resolveMagneticInteractions();

    spaceShipPart->update(dt);

    //step the bullet, update test obj
    bullet->step(dt);

    camera->setPosition(bullet->getBulletObjectState("cam"));

    psystem->update(dt, camera->getPosition());

    if (camera->checkForCollision(spaceShipPart)) {
        //cout << "Collision" << endl;
        kdtree = nullptr;
        bvh = nullptr;
        objects.clear();
        deathObjects.clear();
        fmod->playSound("win", false);
        gameState = WIN;
    }
    /*check for collision with death objects*/
    for (unsigned int i = 0; i < deathObjects.size(); i++) {
        if (camera->checkForCollision(deathObjects.at(i))) {
            objects.clear();
            deathObjects.clear();
            fmod->playSound("death", false);
            gameState = DEATH;
        }
    }
}

void GameManager::drawScene(shared_ptr<Program> shader, shared_ptr<MatrixStack> P,
                            shared_ptr<MatrixStack> V)
{
    //vfc->extractVFPlanes(P->topMatrix(), V->topMatrix());
    for (unsigned int i = 0; i < objects.size(); i++) {
        if (objects.at(i)->isCuboid()) {
            std::shared_ptr<Cuboid> cub = dynamic_pointer_cast<Cuboid>(objects.at(i));
            //std::vector<vec3> *temp = cub->getAabbMinsMaxs();
            //if (!vfc->viewFrustCull(temp)) {
            if (cub->isMagnetic()) {
                program->bind();
                glUniformMatrix4fv(program->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));
                glUniformMatrix4fv(program->getUniform("V"), 1, GL_FALSE, value_ptr(V->topMatrix()));
                glUniform3fv(program->getUniform("lightPos"), 1, value_ptr(vec3(lightPos)));
                glUniform1f(program->getUniform("lightIntensity"), lightIntensity);
                cub->draw(program);
                program->unbind();
            } else {
                shader->bind();
                // NOTE: Depth texture is hard coded at 3. If more textures
                // are added, then this needs to change.
                glActiveTexture(GL_TEXTURE0 + 3);
                glBindTexture(GL_TEXTURE_2D, depthMap);
                glUniform1i(shader->getUniform("shadowDepth"), 3);
                glUniformMatrix4fv(shader->getUniform("LS"), 1, GL_FALSE, value_ptr(LSpace));
                glUniformMatrix4fv(shader->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));
                glUniformMatrix4fv(shader->getUniform("V"), 1, GL_FALSE, value_ptr(V->topMatrix()));
                glUniform3fv(shader->getUniform("lightPos"), 1, value_ptr(vec3(lightPos)));
                glUniform3fv(shader->getUniform("viewPos"), 1, value_ptr(camera->getPosition()));
                glUniform1f(shader->getUniform("lightIntensity"), lightIntensity);
                glUniform3fv(shader->getUniform("scalingFactor"), 1, value_ptr(cub->getScale()));
                cub->draw(shader);
                shader->unbind();
            }
            //}
            
            //delete temp;
        }
    }
}

void GameManager::drawShipPart(shared_ptr<Program> shader, shared_ptr<MatrixStack> P,
                               shared_ptr<MatrixStack> V)
{
    // Draw ship part
    shader->bind();
    glActiveTexture(GL_TEXTURE0 + 3);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glUniform1i(shader->getUniform("shadowDepth"), 3);
    GLSL::checkError(GET_FILE_LINE);
    glUniformMatrix4fv(shader->getUniform("LS"), 1, GL_FALSE, value_ptr(LSpace));
    GLSL::checkError(GET_FILE_LINE);
    shipPartColorTexture->bind(shader->getUniform("diffuseTex"));
    GLSL::checkError(GET_FILE_LINE);
    shipPartSpecularTexture->bind(shader->getUniform("specularTex"));
    glUniformMatrix4fv(shader->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));
    GLSL::checkError(GET_FILE_LINE);
    glUniformMatrix4fv(shader->getUniform("V"), 1, GL_FALSE, value_ptr(V->topMatrix()));
    GLSL::checkError(GET_FILE_LINE);
    glUniform3fv(shader->getUniform("lightPos"), 1, value_ptr(vec3(lightPos)));
    GLSL::checkError(GET_FILE_LINE);
    glUniform3fv(shader->getUniform("viewPos"), 1, value_ptr(camera->getPosition()));
    GLSL::checkError(GET_FILE_LINE);
    spaceShipPart->draw(shader);
    shipPartSpecularTexture->unbind();
    shipPartColorTexture->unbind();
    shader->unbind();
}

mat4 SetOrthoMatrix() {
    mat4 OP = glm::ortho(-30.0, 30.0, -30.0, 30.0, 0.1, 50.0);
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
        //cout << "SHOULD BE HERE" << endl;
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
        
        skybox->render(P, V);

        if (SHADOW) {
            /* BEGIN DEPTH MAP */
            glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
            glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
            glClear(GL_DEPTH_BUFFER_BIT);
            //glCullFace(GL_FRONT);
            
            //set up shadow shader
            //render scene
            DepthProg->bind();
            mat4 LO = SetOrthoMatrix();
            mat4 LV = SetLightView(vec3(lightPos), vec3(0), vec3(0, 1, 0));
            LSpace = LO*LV;
            glUniformMatrix4fv(DepthProg->getUniform("LS"), 1, GL_FALSE, value_ptr(LSpace));
            
            drawScene(DepthProg, P, V);
            drawShipPart(DepthProg, P, V);
            DepthProg->unbind();
            //glCullFace(GL_BACK);
            
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            /* END DEPTH MAP */
        }
        
        glViewport(0, 0, width, height);
        // Clear framebuffer.
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        if (SHADOW) {
            drawScene(skyscraperProgram, P, V);
            drawShipPart(shipPartProgram, P, V);
        }
        else {
//            // Draw ship part
//            shipPartProgram->bind();
//            shipPartColorTexture->bind(shipPartProgram->getUniform("diffuseTex"));
//            shipPartSpecularTexture->bind(shipPartProgram->getUniform("specularTex"));
//            glUniformMatrix4fv(shipPartProgram->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));
//            glUniformMatrix4fv(shipPartProgram->getUniform("V"), 1, GL_FALSE, value_ptr(V->topMatrix()));
//            glUniform4fv(shipPartProgram->getUniform("lightPos"), 1, value_ptr(lightPos));
//            spaceShipPart->draw(shipPartProgram);
//            shipPartSpecularTexture->unbind();
//            shipPartColorTexture->unbind();
//            shipPartProgram->unbind();
            
            drawShipPart(shipPartProgram, P, V);
            drawScene(skyscraperProgram, P, V);
            
            //        vfc->extractVFPlanes(P->topMatrix(), V->topMatrix());
            //        for (unsigned int i = 0; i < objects.size(); i++) {
            //            if (objects.at(i)->isCuboid()) {
            //                std::shared_ptr<Cuboid> cub = dynamic_pointer_cast<Cuboid>(objects.at(i));
            //                std::vector<vec3> *temp = cub->getAabbMinsMaxs();
            //
            //                if (!vfc->viewFrustCull(temp)) {
            //                    objectsDrawn++;
            //                    if (cub->isMagnetic()) {
            //                        program->bind();
            //                        glUniformMatrix4fv(program->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));
            //                        glUniformMatrix4fv(program->getUniform("V"), 1, GL_FALSE, value_ptr(V->topMatrix()));
            //                        glUniform4fv(program->getUniform("lightPos"), 1, value_ptr(l));
            //                        glUniform1f(program->getUniform("lightIntensity"), lightIntensity);
            //                        cub->draw(program);
            //                        program->unbind();
            //                    } else {
            //                        skyscraperProgram->bind();
            //                        glUniformMatrix4fv(skyscraperProgram->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));
            //                        glUniformMatrix4fv(skyscraperProgram->getUniform("V"), 1, GL_FALSE, value_ptr(V->topMatrix()));
            //                        glUniform4fv(skyscraperProgram->getUniform("lightPos"), 1, value_ptr(lightPos));
            //                        glUniform1f(skyscraperProgram->getUniform("lightIntensity"), lightIntensity);
            //                        glUniform3fv(skyscraperProgram->getUniform("scalingFactor"), 1, value_ptr(cub->getScale()));
            //                        cub->draw(skyscraperProgram);
            //                        skyscraperProgram->unbind();
            //                    }
            //                }
            //
            //                delete temp;
            //            }
            //        }
            
            // Render magnet gun
            program->bind();
            glUniformMatrix4fv(program->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));
            glUniform4fv(program->getUniform("lightPos"), 1, value_ptr(lightPos));
            glUniform1f(program->getUniform("lightIntensity"), lightIntensity);
            V->pushMatrix();
            V->loadIdentity();
            glUniformMatrix4fv(program->getUniform("V"), 1, GL_FALSE, value_ptr(V->topMatrix()));
            glClear(GL_DEPTH_BUFFER_BIT);
            magnetGun->draw(program);
            V->popMatrix();
            
            if (drawBeam) {
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                glUniform1f(program->getUniform("lightIntensity"), 0.9f);
                if (colorBeam == BLUE) {
                    magnetBeamBlue->draw(program);
                    psystem->setColor(BLUE);
                } else {
                    magnetBeamOrange->draw(program);
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
            program->unbind();
        }
        
        if (SHADOW_DEBUG) {
            glClear( GL_DEPTH_BUFFER_BIT);
            glViewport(0, 0, 300, 300);
            DebugProg->bind();
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, depthMap);
            glUniform1i(DebugProg->getUniform("texBuf"), 0);
            glEnableVertexAttribArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *) 0);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glDisableVertexAttribArray(0);
            DebugProg->unbind();
            glViewport(0, 0, width, height);
        }
        
        V->popMatrix();
        P->popMatrix();
        
        if (gameState == PAUSE) {
            gui->drawPause(level);
        } else {
            gui->drawHUD(camera->isLookingAtMagnet(), Mouse::isLeftMouseButtonPressed(), Mouse::isRightMouseButtonPressed());
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

    //vector<shared_ptr<GameObject>> nearObjs = kdtree->findObjectsIntersectedByRay(startLoc, endLoc);
    //shared_ptr<GameObject> obj = RayTrace::rayTrace(startLoc, endLoc, objects);
    shared_ptr<GameObject> obj = bvh->findClosestHitObject(startLoc, endLoc);
    drawBeam = false;
    if (obj && obj->isMagnetic()) {
        camera->setLookingAtMagnet(true);
        if (Mouse::isLeftMouseButtonPressed()) {
            if (!fmod->isPlaying("magnet")) {
                fmod->playSound("magnet", false, 1);
            }
            vec3 dir = normalize(endLoc - startLoc);
            btVector3 bulletDir = btVector3(dir.x, dir.y, dir.z);
            bullet->getBulletObject("cam")->getRigidBody()->setLinearVelocity(bulletDir * MAGNET_STRENGTH);
            drawBeam = true;
            colorBeam = ORANGE;
        } else if (Mouse::isRightMouseButtonPressed()) {
            if (!fmod->isPlaying("magnet")) {
                fmod->playSound("magnet", false, 1);
            }
            vec3 dir = normalize(startLoc - endLoc);
            btVector3 bulletDir = btVector3(dir.x, dir.y, dir.z);
            bullet->getBulletObject("cam")->getRigidBody()->setLinearVelocity(bulletDir * MAGNET_STRENGTH);
            drawBeam = true;
            colorBeam = BLUE;
        }

    } else {
        camera->setLookingAtMagnet(false);
        drawBeam = false;
        if (Mouse::isLeftMouseButtonPressed() || Mouse::isRightMouseButtonPressed()) {
            if (!fmod->isPlaying("click"))
                fmod->playSound("click", false, 1);
        }
    }
}

// Returns a random float between the provided low and high floats.

float GameManager::randFloat(float l, float h) {
    float r = rand() / (float) RAND_MAX;
    return (1.0f - r) * l + r * h;
}
