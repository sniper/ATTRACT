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
#include <fstream>

#include "GuiManager.hpp"
#include "ParticleManager.hpp"

#include "KDTree.hpp"
#include "BVH.hpp"

#include "stb_easy_font.h"

#define MAX_NUM_OBJECTS 15
#define GRID_SIZE 8
#define OBJ_SPAWN_INTERVAL 2
#define BUNNY_SPHERE_RADIUS 0.5f
#define MAGNET_RANGE 13.0f
#define MAGNET_STRENGTH 7.0f
#define CUBE_HALF_EXTENTS vec3(0.5f, 0.5f, 0.5f)

using namespace std;
using namespace glm;

GameManager::GameManager(GLFWwindow *window, const string &resourceDir) :
window(window),
RESOURCE_DIR(resourceDir),
gameState(MENU),
level(1) {
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

    // Initialize the scene.
    initScene();
}

GameManager::~GameManager() {

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
    program->addUniform("MV");
    program->addUniform("P");
    program->addUniform("lightPos");
    program->addUniform("lightIntensity");
    program->addUniform("ka");
    program->addUniform("kd");
    program->addUniform("ks");
    program->addUniform("s");
    program->addUniform("objTransMatrix");
    //
    // Ship Parts (with Texture)
    //
    shipPartProgram = make_shared<Program>();
    shipPartProgram->setShaderNames(RESOURCE_DIR + "shipPartVert.glsl",
            RESOURCE_DIR + "shipPartFrag.glsl");
    shipPartProgram->setVerbose(true);
    shipPartProgram->init();

    shipPartProgram->addAttribute("aPos");
    shipPartProgram->addAttribute("aNor");
    shipPartProgram->addAttribute("aTex");
    shipPartProgram->addUniform("P");
    shipPartProgram->addUniform("MV");
    shipPartProgram->addUniform("colorTexture");
    shipPartProgram->addUniform("specularTexture");
    shipPartProgram->addUniform("lightPos");
    shipPartProgram->addUniform("objTransMatrix");

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
    shared_ptr<Shape> cube = make_shared<Shape>();
    cube->loadMesh(RESOURCE_DIR + "cube.obj");
    cube->fitToUnitBox();
    cube->init();
    shapes.push_back(cube);

    shared_ptr<Shape> shipPart = make_shared<Shape>();
    shipPart->loadMesh(RESOURCE_DIR + "shipPart.obj");
    shipPart->fitToUnitBox();
    shipPart->init();
    shapes.push_back(shipPart);

    shared_ptr<Shape> gun = make_shared<Shape>();
    gun->loadMesh(RESOURCE_DIR + "MP5K.obj");
    gun->fitToUnitBox();
    gun->init();
    shapes.push_back(gun);

    //lightPos = vec4(0.0f, 10.0f, 0.0f, 1.0f);
    lightIntensity = 0.6f;

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    GLSL::checkError(GET_FILE_LINE);

    // Set up the bullet manager and create ground plane and camera.
    //bullet = make_shared<BulletManager>();
    //bullet->createPlane("ground", 0, 0, 0);

    shared_ptr<Material> material = make_shared<Material>(vec3(0.2f, 0.2f, 0.2f), vec3(0.0f, 0.5f, 0.5f), vec3(1.0f, 0.9f, 0.8f), 200.0f);
    magnetGun = make_shared<GameObject>(vec3(0.4, -0.2, -1), vec3(0.4, 0, -0.2), vec3(0.5, 0.5, 0.5), 0, shapes.at(2), material);
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
                scale, 0, shapes.at(0),
                magnetSurface, true);
        objects.push_back(magnet);
        bullet->createMagneticBox(to_string(name++), pos, CUBE_HALF_EXTENTS, scale, 0);
    } else if (deadly) {
        shared_ptr<Cuboid> dobj1 = make_shared<Cuboid>(pos, vec3(0, 0, 0),
                CUBE_HALF_EXTENTS,
                scale, 0, shapes.at(0),
                greyBox, false);
        deathObjects.push_back(dobj1);

        //cout << "death box" << endl;
    } else if (collectable) {
        spaceShipPart = make_shared<SpaceShipPart>(pos, vec3(0, 0, 0),
                CUBE_HALF_EXTENTS, scale,
                shapes.at(1), spacePart);
    } else {
        shared_ptr<Cuboid> groundPlane = make_shared<Cuboid>(pos, vec3(0, 0, 0),
                CUBE_HALF_EXTENTS,
                scale, 0, shapes.at(0),
                greyBox, false);
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

    //kdtree = make_shared<KDTree>(objects);
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

    // Matrix stacks
    auto P = make_shared<MatrixStack>();
    auto MV = make_shared<MatrixStack>();

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
        MV->pushMatrix();
        camera->applyViewMatrix(MV);



        lightPos = vec4(camera->getPosition(), 1.0);
        vec4 l = MV->topMatrix() * lightPos;

        // Draw ship part
        shipPartProgram->bind();
        shipPartColorTexture->bind(shipPartProgram->getUniform("colorTexture"));
        shipPartSpecularTexture->bind(shipPartProgram->getUniform("specularTexture"));
        glUniformMatrix4fv(shipPartProgram->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));
        glUniformMatrix4fv(shipPartProgram->getUniform("MV"), 1, GL_FALSE, value_ptr(MV->topMatrix()));
        glUniform3fv(shipPartProgram->getUniform("lightPos"), 1, value_ptr(vec3(l)));
        spaceShipPart->draw(shipPartProgram);
        shipPartSpecularTexture->unbind();
        shipPartColorTexture->unbind();
        shipPartProgram->unbind();

        // Render objects
        program->bind();
        glUniformMatrix4fv(program->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));
        glUniformMatrix4fv(program->getUniform("MV"), 1, GL_FALSE, value_ptr(MV->topMatrix()));
        glUniform4f(program->getUniform("lightPos"), l[0], l[1], l[2], l[3]);
        glUniform1f(program->getUniform("lightIntensity"), lightIntensity);

        vfc->extractVFPlanes(P->topMatrix(), MV->topMatrix());
        for (unsigned int i = 0; i < objects.size(); i++) {
            //this will fail if we have different types TODO: FIX THIS
            //meme leaks????
            std::shared_ptr<Cuboid> cub = dynamic_pointer_cast<Cuboid>(objects.at(i));
            std::vector<vec3> *temp = cub->getAabbMinsMaxs();

            if (!vfc->viewFrustCull(temp)) {
                objectsDrawn++;
                objects.at(i)->draw(program);
            }

            delete temp;
        }

        if (bullet->getDebugFlag()) {
            /*DRAW DEATH OBJECTS*/
            for (unsigned int i = 0; i < deathObjects.size(); i++) {
                deathObjects.at(i)->draw(program);

            }
        }
        // Draw magnet gun
        MV->pushMatrix();
        MV->loadIdentity();
        glUniformMatrix4fv(program->getUniform("MV"), 1, GL_FALSE, value_ptr(MV->topMatrix()));
        glDisable(GL_DEPTH_TEST);
        magnetGun->draw(program);
        glEnable(GL_DEPTH_TEST);
        MV->popMatrix();


        if (bullet->getDebugFlag()) {

            for (unsigned int i = 0; i < deathObjects.size(); i++) {

            }
        }
        program->unbind();

        psystem->draw( MV->topMatrix() , P->topMatrix(), 0);



        if (bullet->getDebugFlag())
            bullet->renderDebug(P->topMatrix(), MV->topMatrix());




        MV->popMatrix();
        P->popMatrix();


        if (gameState == PAUSE) {
            gui->drawPause(level);

        }



    }



    //
    // stb_easy_font.h is used for printing fonts to the screen.
    //

    // Prints a crosshair to the center of the screen. Color depends on if you're looking at a magnet surface.

    /*
    if (camera->isLookingAtMagnet()) {
        if (Mouse::isLeftMouseButtonPressed()) {
            printStringToScreen(0.0f, 0.0f, "+", 0.0f, 1.0f, 1.0f);
        } else if (Mouse::isRightMouseButtonPressed()) {
            printStringToScreen(0.0f, 0.0f, "+", 1.0f, 0.6f, 0.0f);
        } else {
            printStringToScreen(0.0f, 0.0f, "+", 0.0f, 1.0f, 0.0f);
        }
    } else {
        printStringToScreen(0.0f, 0.0f, "+", 0.0f, 0.0f, 0.0f);
    }
    // Prints the frame rate to the screen.
    printStringToScreen(60.0f, 90.0f, to_string(fps) + " FPS", 0.0f, 0.0f, 0.0f);
     */
    GLSL::checkError(GET_FILE_LINE);
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
    if (obj && obj->isMagnetic()) {
        camera->setLookingAtMagnet(true);
        if (Mouse::isLeftMouseButtonPressed()) {
            if (!fmod->isPlaying("magnet")) {
                fmod->playSound("magnet", false, 1);
            }
            vec3 dir = normalize(endLoc - startLoc);
            btVector3 bulletDir = btVector3(dir.x, dir.y, dir.z);
            bullet->getBulletObject("cam")->getRigidBody()->setLinearVelocity(bulletDir * MAGNET_STRENGTH);
        } else if (Mouse::isRightMouseButtonPressed()) {
            if (!fmod->isPlaying("magnet")) {
                fmod->playSound("magnet", false, 1);
            }
            vec3 dir = normalize(startLoc - endLoc);
            btVector3 bulletDir = btVector3(dir.x, dir.y, dir.z);
            bullet->getBulletObject("cam")->getRigidBody()->setLinearVelocity(bulletDir * MAGNET_STRENGTH);
        }

    } else {
        camera->setLookingAtMagnet(false);
        if (Mouse::isLeftMouseButtonPressed() || Mouse::isRightMouseButtonPressed()) {
            if (!fmod->isPlaying("click"))
                fmod->playSound("click", false, 1);
        }
    }
}

void GameManager::printStringToScreen(float x, float y, const string &text, float r, float g, float b) {
    static char buffer[99999]; // ~500 chars
    int num_quads;

    char *toScreen = new char[text.size() + 1];
    strcpy(toScreen, text.c_str());

    // With this disabled, text will always be printed to the screen, even if
    // an object is "in front" of it.
    glDisable(GL_DEPTH_TEST);

    //
    glPushMatrix();
    num_quads = stb_easy_font_print(x, y, toScreen, NULL, buffer, sizeof (buffer));
    glScalef(0.01f, -0.01f, 1.0f);
    glColor3f(r, g, b);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 16, buffer);
    glDrawArrays(GL_QUADS, 0, num_quads * 4);
    glDisableClientState(GL_VERTEX_ARRAY);
    glPopMatrix();

    // Re-enable depth test for further render calls.
    glEnable(GL_DEPTH_TEST);

    delete[] toScreen;
}

// Returns a random float between the provided low and high floats.

float GameManager::randFloat(float l, float h) {
    float r = rand() / (float) RAND_MAX;
    return (1.0f - r) * l + r * h;
}
