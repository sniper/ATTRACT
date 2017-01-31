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

#include "stb_easy_font.h"

#include "GameManager.hpp"
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
#include "BulletManager.h"

#define MAX_NUM_OBJECTS 15
#define GRID_SIZE 8
#define OBJ_SPAWN_INTERVAL 2
#define BUNNY_SPHERE_RADIUS 0.5f
#define MAGNET_RANGE 13.0f

using namespace std;
using namespace glm;

GameManager::GameManager(GLFWwindow *window, const string &resourceDir) :
window(window),
RESOURCE_DIR(resourceDir) {
    objIntervalCounter = 0.0f;
    numObjCollected = 0;
    gameWon = false;

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
    // Create the camera for the scene.
    camera = make_shared<Camera>(GRID_SIZE);
    // Set up the manager for keyboard and mouse input.
    inputManager = make_shared<InputManager>(camera);
    // Initialize the scene.
    initScene();
    //create the level
    createLevel();
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

    shared_ptr<Shape> bunny = make_shared<Shape>();
    bunny->loadMesh(RESOURCE_DIR + "bunny.obj");
    bunny->fitToUnitBox();
    bunny->init();
    shapes.push_back(bunny);

    shared_ptr<Shape> cube = make_shared<Shape>();
    cube->loadMesh(RESOURCE_DIR + "cube.obj");
    cube->fitToUnitBox();
    cube->init();
    shapes.push_back(cube);

    //lightPos = vec4(0.0f, 10.0f, 0.0f, 1.0f);
    lightIntensity = 0.6f;

    //
    // Sun
    //
    sunProg = make_shared<Program>();
    sunProg->setShaderNames(RESOURCE_DIR + "sunVert.glsl", RESOURCE_DIR + "sunFrag.glsl");
    sunProg->setVerbose(false);
    sunProg->init();
    sunProg->addAttribute("aPos");
    sunProg->addUniform("MV");
    sunProg->addUniform("P");

    sun = make_shared<Shape>();
    sun->loadMesh(RESOURCE_DIR + "sphere.obj");
    sun->fitToUnitBox();
    sun->init();

    //
    // Grass Texture
    //
    ground = make_shared<Program>();
    ground->setShaderNames(RESOURCE_DIR + "grassVert.glsl", RESOURCE_DIR + "grassFrag.glsl");
    ground->setVerbose(false);
    ground->init();
    ground->addAttribute("aPos");
    ground->addAttribute("aTex");
    ground->addUniform("MV");
    ground->addUniform("P");
    ground->addUniform("grassTexture");

    grass = make_shared<Texture>();
    grass->setFilename(RESOURCE_DIR + "grass.jpg");
    grass->init();
    grass->setUnit(0);
    grass->setWrapModes(GL_REPEAT, GL_REPEAT);

    // 0
    grassPosBuf.push_back(-GRID_SIZE);
    grassPosBuf.push_back(0.0f);
    grassPosBuf.push_back(-GRID_SIZE);
    grassTexBuf.push_back(0.0f);
    grassTexBuf.push_back(0.0f);
    // 1
    grassPosBuf.push_back(GRID_SIZE);
    grassPosBuf.push_back(0.0f);
    grassPosBuf.push_back(-GRID_SIZE);
    grassTexBuf.push_back(30.0f);
    grassTexBuf.push_back(0.0f);
    // 2
    grassPosBuf.push_back(-GRID_SIZE);
    grassPosBuf.push_back(0.0f);
    grassPosBuf.push_back(GRID_SIZE);
    grassTexBuf.push_back(0.0f);
    grassTexBuf.push_back(30.0f);
    // 3
    grassPosBuf.push_back(GRID_SIZE);
    grassPosBuf.push_back(0.0f);
    grassPosBuf.push_back(GRID_SIZE);
    grassTexBuf.push_back(30.0f);
    grassTexBuf.push_back(30.0f);
    // Index
    grassIndBuf.push_back(0);
    grassIndBuf.push_back(1);
    grassIndBuf.push_back(2);
    grassIndBuf.push_back(3);
    grassIndBuf.push_back(2);
    grassIndBuf.push_back(1);
    grassIndCount = (int) grassIndBuf.size();

    GLuint tmp[3];
    glGenBuffers(3, tmp);
    grassBufIDs["bPos"] = tmp[0];
    grassBufIDs["bTex"] = tmp[1];
    grassBufIDs["bInd"] = tmp[2];

    glBindBuffer(GL_ARRAY_BUFFER, grassBufIDs["bPos"]);
    glBufferData(GL_ARRAY_BUFFER, grassPosBuf.size() * sizeof (float), &grassPosBuf[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, grassBufIDs["bTex"]);
    glBufferData(GL_ARRAY_BUFFER, grassTexBuf.size() * sizeof (float), &grassTexBuf[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, grassBufIDs["bInd"]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, grassIndBuf.size() * sizeof (unsigned int), &grassIndBuf[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    GLSL::checkError(GET_FILE_LINE);

    // Set up the bullet manager and create ground plane and camera.
    bullet = new BulletManager();
    bullet->createPlane("ground", 0, 0, 0);

    bullet->createBox("cam", vec3(0, 0.5, 0), vec3(0.5, 0.5, 0.5), vec3(1,1,1), 1);

    //shared_ptr<Material> material = make_shared<Material>(vec3(0.2f, 0.2f, 0.2f), vec3(0.0f, 0.5f, 0.5f), vec3(1.0f, 0.9f, 0.8f), 200.0f);
    //shared_ptr<BoundingSphere> boundingSphere = make_shared<BoundingSphere>(vec3(0, 10, 0), BUNNY_SPHERE_RADIUS);
    //testObj = make_shared<GameObject>(vec3(0, 10, 0), vec3(1, 0, 0), 0, boundingSphere, shapes.at(0), material);
    //objects.push_back(testObj);
}

void GameManager::createLevel()
{
    shared_ptr<Material> building = make_shared<Material>(vec3(0.9f, 0.9f, 0.9f), vec3(1.0f, 1.0f, 1.0f), vec3(0.0f, 0.0f, 0.0f), 200.0f);
    shared_ptr<Material> ground = make_shared<Material>(vec3(0.6f, 0.6f, 0.6f), vec3(0.7f, 0.7f, 0.7f), vec3(0.0f, 0.0f, 0.0f), 200.0f);
    shared_ptr<Material> greyBox = make_shared<Material>(vec3(0.2f, 0.2f, 0.2f), vec3(0.4f, 0.4f, 0.4f), vec3(0.0f, 0.0f, 0.0f), 200.0f);
    shared_ptr<Material> magnetSurface = make_shared<Material>(vec3(0.2f, 0.2f, 0.2f), vec3(1.0f, 0.0f, 0.0f), vec3(1.0f, 0.9f, 0.8f), 200.0f);
    vec3 location, direction, scale, dimensions;
    
    // Ground Plane
    location = vec3(20, 0, 0);
    direction = vec3(1, 0, 0);
    scale = vec3(100, 0.1, 10);
    dimensions = vec3(0.5, 0.5, 0.5);
    shared_ptr<GameObject> groundPlane = make_shared<GameObject>(location, direction, scale, 0, make_shared<BoundingSphere>(), shapes.at(1), ground);
    objects.push_back(groundPlane);
    bullet->createBox("groundPlane", location, dimensions, scale, 0);
    
    //
    // Walls
    //
    location = vec3(20, 2, 3);
    direction = vec3(1, 0, 0);
    scale = vec3(70, 50, 1);
    dimensions = vec3(0.5, 0.5, 0.5);
    shared_ptr<GameObject> rightWall = make_shared<GameObject>(location, direction, scale, 0, make_shared<BoundingSphere>(), shapes.at(1), building);
    objects.push_back(rightWall);
    bullet->createBox("rightWall", location, dimensions, scale, 0);

    location = vec3(20, 2, -3);
    direction = vec3(1, 0, 0);
    scale = vec3(70, 50, 1);
    dimensions = vec3(0.5, 0.5, 0.5);
    shared_ptr<GameObject> leftWall = make_shared<GameObject>(location, direction, scale, 0, make_shared<BoundingSphere>(), shapes.at(1), building);
    objects.push_back(leftWall);
    bullet->createBox("leftWall", location, dimensions, scale, 0);
    
    location = vec3(-1, 2, 0);
    direction = vec3(1, 0, 0);
    scale = vec3(1, 50, 5);
    dimensions = vec3(0.5, 0.5, 0.5);
    shared_ptr<GameObject> behindWall = make_shared<GameObject>(location, direction, scale, 0, make_shared<BoundingSphere>(), shapes.at(1), building);
    objects.push_back(behindWall);
    bullet->createBox("behindWall", location, dimensions, scale, 0);
    
    location = vec3(53, 2, 0);
    direction = vec3(1, 0, 0);
    scale = vec3(1, 50, 5);
    dimensions = vec3(0.5, 0.5, 0.5);
    shared_ptr<GameObject> endWall = make_shared<GameObject>(location, direction, scale, 0, make_shared<BoundingSphere>(), shapes.at(1), building);
    objects.push_back(endWall);
    bullet->createBox("endWall", location, dimensions, scale, 0);
    
    //
    // First Room
    //
    location = vec3(7, 1, 0);
    direction = vec3(1, 0, 0);
    scale = vec3(5, 3, 5);
    dimensions = vec3(0.5, 0.5, 0.5);
    shared_ptr<GameObject> firstPlatform = make_shared<GameObject>(location, direction, scale, 0, make_shared<BoundingSphere>(), shapes.at(1), building);
    objects.push_back(firstPlatform);
    bullet->createBox("firstPlatform", location, dimensions, scale, 0);
    
    location = vec3(3, 0.5, 0);
    direction = vec3(1, 0, 0);
    scale = vec3(1, 1, 1);
    dimensions = vec3(0.5, 0.5, 0.5);
    shared_ptr<GameObject> startingBox = make_shared<GameObject>(location, direction, scale, 0, make_shared<BoundingSphere>(), shapes.at(1), greyBox);
    objects.push_back(startingBox);
    bullet->createBox("startingBox", location, dimensions, scale, 0);
    
    location = vec3(4.7, 4.2, 3);
    direction = vec3(1, 0, 0);
    scale = vec3(1, 1, 1.5);
    dimensions = vec3(0.5, 0.5, 0.5);
    shared_ptr<GameObject> rightTopPad = make_shared<GameObject>(location, direction, scale, 0, make_shared<BoundingSphere>(), shapes.at(1), magnetSurface);
    objects.push_back(rightTopPad);
    bullet->createMagneticBox("rightTopPad", location, dimensions, scale, 0);

    //
    // Jump that you can make
    //
    location = vec3(14, 1, 0);
    direction = vec3(1, 0, 0);
    scale = vec3(5, 3, 5);
    dimensions = vec3(0.5, 0.5, 0.5);
    shared_ptr<GameObject> secondPlatform = make_shared<GameObject>(location, direction, scale, 0, make_shared<BoundingSphere>(), shapes.at(1), building);
    objects.push_back(secondPlatform);
    bullet->createBox("secondPlatform", location, dimensions, scale, 0);
    
    location = vec3(10, 0.5, 0);
    direction = vec3(1, 0, 0);
    scale = vec3(1, 1, 1);
    dimensions = vec3(0.5, 0.5, 0.5);
    shared_ptr<GameObject> pitBox = make_shared<GameObject>(location, direction, scale, 0, make_shared<BoundingSphere>(), shapes.at(1), greyBox);
    objects.push_back(pitBox);
    bullet->createBox("pitBox", location, dimensions, scale, 0);
    
    //
    // Jump that you can't make but with magnet in pit
    //
    location = vec3(25, 1, 0);
    direction = vec3(1, 0, 0);
    scale = vec3(5, 3, 5);
    dimensions = vec3(0.5, 0.5, 0.5);
    shared_ptr<GameObject> thirdPlatform = make_shared<GameObject>(location, direction, scale, 0, make_shared<BoundingSphere>(), shapes.at(1), building);
    objects.push_back(thirdPlatform);
    bullet->createBox("thirdPlatform", location, dimensions, scale, 0);

    location = vec3(20, 0, 0);
    direction = vec3(1, 0, 0);
    scale = vec3(1, 0.2, 1);
    dimensions = vec3(0.5, 0.5, 0.5);
    shared_ptr<GameObject> magnetPadInPit = make_shared<GameObject>(location, direction, scale, 0, make_shared<BoundingSphere>(), shapes.at(1), magnetSurface);
    objects.push_back(magnetPadInPit);
    bullet->createMagneticBox("magnetPadInPit", location, dimensions, scale, 0);
    
    //
    // Long hallway with pull magnets
    //
    location = vec3(50, 2, 0);
    direction = vec3(1, 0, 0);
    scale = vec3(5, 4, 5);
    dimensions = vec3(0.5, 0.5, 0.5);
    shared_ptr<GameObject> finalPlatform = make_shared<GameObject>(location, direction, scale, 0, make_shared<BoundingSphere>(), shapes.at(1), building);
    objects.push_back(finalPlatform);
    bullet->createBox("finalPlatform", location, dimensions, scale, 0);
    
    location = vec3(29, 0.5, 0);
    direction = vec3(1, 0, 0);
    scale = vec3(1, 1, 1);
    dimensions = vec3(0.5, 0.5, 0.5);
    shared_ptr<GameObject> hallwayBox = make_shared<GameObject>(location, direction, scale, 0, make_shared<BoundingSphere>(), shapes.at(1), greyBox);
    objects.push_back(hallwayBox);
    bullet->createBox("hallwayBox", location, dimensions, scale, 0);
    
    location = vec3(30.7, 13, 3);
    direction = vec3(1, 0, 0);
    scale = vec3(1.5, 1.5, 1.5);
    dimensions = vec3(0.5, 0.5, 0.5);
    shared_ptr<GameObject> hallwayFirstPad = make_shared<GameObject>(location, direction, scale, 0, make_shared<BoundingSphere>(), shapes.at(1), magnetSurface);
    objects.push_back(hallwayFirstPad);
    bullet->createMagneticBox("hallwayFirstPad", location, dimensions, scale, 0);
    
    location = vec3(40.7, 13, -3);
    direction = vec3(1, 0, 0);
    scale = vec3(1.5, 1.5, 1.5);
    dimensions = vec3(0.5, 0.5, 0.5);
    shared_ptr<GameObject> hallwaySecondPad = make_shared<GameObject>(location, direction, scale, 0, make_shared<BoundingSphere>(), shapes.at(1), magnetSurface);
    objects.push_back(hallwaySecondPad);
    bullet->createMagneticBox("hallwaySecondPad", location, dimensions, scale, 0);
    
    location = vec3(52, 4, 0);
    direction = vec3(1, 0, 0);
    scale = vec3(0.1, 1.5, 1.5);
    dimensions = vec3(0.5, 0.5, 0.5);
    shared_ptr<GameObject> hallwayLastPad = make_shared<GameObject>(location, direction, scale, 0, make_shared<BoundingSphere>(), shapes.at(1), magnetSurface);
    objects.push_back(hallwayLastPad);
    bullet->createMagneticBox("hallwayLastPad", location, dimensions, scale, 0);

//    location = vec3(3, 0, -1);
//    direction = vec3(1, 0, 0);
//    scale = vec3(1, 0.2, 1);
//    dimensions = vec3(0.5, 0.5, 0.5);
//    shared_ptr<GameObject> leftPad = make_shared<GameObject>(location, direction, scale, 0, make_shared<BoundingSphere>(), shapes.at(1), magnetSurface);
//    objects.push_back(leftPad);
//    bullet->createMagneticBox("leftPad", location, dimensions, scale, 0);

//    location = vec3(-4.7, 4.2, 3);
//    direction = vec3(1, 0, 0);
//    scale = vec3(0.5, 0.5, 1.5);
//    dimensions = vec3(0.5, 0.5, 0.5);
//    shared_ptr<GameObject> rightTopPad = make_shared<GameObject>(location, direction, scale, 0, make_shared<BoundingSphere>(), shapes.at(1), magnetSurface);
//    objects.push_back(rightTopPad);
//    bullet->createMagneticBox("rightTopPad", location, dimensions, scale, 0);
}

void GameManager::processInputs() {
    inputManager->processInputs(bullet);
}

void GameManager::updateGame(double dt) {

    bullet->rayTrace(camera->getPosition(), camera->getPosition() + (camera->getDirection() * MAGNET_RANGE));

    //step the bullet, update test obj
    bullet->step(dt);

    camera->setPosition(bullet->getBulletObjectState("cam"));

    // Spawn in a bunny every OBJ_SPAWN_INTERVAL seconds
    //    objIntervalCounter += dt;
    //    if (objIntervalCounter > OBJ_SPAWN_INTERVAL && objects.size() < MAX_NUM_OBJECTS) {
    //        objIntervalCounter = 0.0;
    //        createBunny();
    //    }


    /*
    for (int i = 0; i < objects.size(); i++) {
        shared_ptr<GameObject> currObj = objects.at(i);

        // Update the current object
        currObj->update(dt);

        // Check for a collision between this object and the player
        if (camera->checkForCollision(currObj)) {
            // If this is the first time the player has contacted this object,
            // then count it as a new object collected.
            if (!currObj->isCollected()) {
                numObjCollected++;
                if (numObjCollected == MAX_NUM_OBJECTS) {
                    gameWon = true;
                }
            }

            camera->resolveCollision();
            currObj->resolveCollision(true);
        }

        vec3 objPosition = currObj->getPosition();
        vec3 objDirection = currObj->getDirection();
        // Check to see if the object is leaving the grid. If it is, then make it
        // collide with the edge of the grid. I slightly alter the GRID_SIZE so the
        // objects don't hang over the edge of the grid.
        if ((objPosition[0] <= -GRID_SIZE + 0.5f && objDirection[0] < 0)
                || (objPosition[0] >= GRID_SIZE - 0.5f && objDirection[0] > 0)
                || (objPosition[2] <= -GRID_SIZE + 0.5f && objDirection[2] < 0)
                || (objPosition[2] >= GRID_SIZE - 0.5f && objDirection[2] > 0)) {
            currObj->resolveCollision(false);
        }

        // Check for collision between this object and the other objects
        for (int j = 0; j < objects.size(); j++) {
            // First, make sure you aren't comparing the object against itself.
            // Then, check for a collision between this object and one of the other objects.
            if (i != j && currObj->isCollidingWithOtherObject(objects.at(j))) {
                currObj->resolveCollision(false);
                objects.at(j)->resolveCollision(false);
            }
        }
    }
     */
}

void GameManager::renderGame(int fps) {
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

    // Apply camera transforms
    P->pushMatrix();
    camera->applyProjectionMatrix(P);
    MV->pushMatrix();
    camera->applyViewMatrix(MV);

    // Render ground
//    ground->bind();
//    grass->bind(ground->getUniform("grassTexture"));
//    glUniformMatrix4fv(ground->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));
//    glUniformMatrix4fv(ground->getUniform("MV"), 1, GL_FALSE, value_ptr(MV->topMatrix()));
//    glEnableVertexAttribArray(ground->getAttribute("aPos"));
//    glBindBuffer(GL_ARRAY_BUFFER, grassBufIDs["bPos"]);
//    glVertexAttribPointer(ground->getAttribute("aPos"), 3, GL_FLOAT, GL_FALSE, 0, (void *) 0);
//    glEnableVertexAttribArray(ground->getAttribute("aTex"));
//    glBindBuffer(GL_ARRAY_BUFFER, grassBufIDs["bTex"]);
//    glVertexAttribPointer(ground->getAttribute("aTex"), 2, GL_FLOAT, GL_FALSE, 0, (void *) 0);
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, grassBufIDs["bInd"]);
//    glDrawElements(GL_TRIANGLES, grassIndCount, GL_UNSIGNED_INT, (void *) 0);
//    grass->unbind();
//    ground->unbind();

    // Render objects
    program->bind();
    glUniformMatrix4fv(program->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));
    glUniformMatrix4fv(program->getUniform("MV"), 1, GL_FALSE, value_ptr(MV->topMatrix()));
    lightPos = vec4(camera->getPosition(), 1.0);
    vec4 l = MV->topMatrix() * lightPos;
    glUniform4f(program->getUniform("lightPos"), l[0], l[1], l[2], l[3]);
    glUniform1f(program->getUniform("lightIntensity"), lightIntensity);
    for (unsigned int i = 0; i < objects.size(); i++) {
        objects.at(i)->draw(program);
    }
    program->unbind();

    // Render sun
//    sunProg->bind();
//    MV->pushMatrix();
//    MV->translate(vec3(0.0f, 10.0f, 0.0f));
//    glUniformMatrix4fv(sunProg->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));
//    glUniformMatrix4fv(sunProg->getUniform("MV"), 1, GL_FALSE, value_ptr(MV->topMatrix()));
//    sun->draw(sunProg);
//    MV->popMatrix();
//    sunProg->unbind();

    printStringToScreen(0.0f, 0.0f, "+", 0.0f, 0.0f, 0.0f);

    //
    // stb_easy_font.h is used for printing fonts to the screen.
    //
    
    // Prints a crosshair to the center of the screen. Color depends on if you're looking at a magnet surface.
    if (bullet->isLookingAtMagnetMaterial()) {
        if (Mouse::isLeftMouseButtonPressed()) {
            printStringToScreen(0.0f, 0.0f, "+", 0.0f, 1.0f, 1.0f);
        }
        else if (Mouse::isRightMouseButtonPressed()) {
            printStringToScreen(0.0f, 0.0f, "+", 1.0f, 0.6f, 0.0f);
        }
        else {
            printStringToScreen(0.0f, 0.0f, "+", 0.0f, 1.0f, 0.0f);
        }
    }
    else {
        printStringToScreen(0.0f, 0.0f, "+", 0.0f, 0.0f, 0.0f);
    }
    // Prints the frame rate to the screen.
    printStringToScreen(60.0f, 90.0f, to_string(fps) + " FPS", 0.0f, 0.0f, 0.0f);

    MV->popMatrix();
    P->popMatrix();

    GLSL::checkError(GET_FILE_LINE);
}

// If the window is resized, capture the new size and reset the viewport

void GameManager::resize_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
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
