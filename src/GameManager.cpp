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
#include "VfcManager.hpp"
#include "GuiManager.hpp"
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
gameState(MENU) {
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
    //init vfc
    vfc = new VfcManager();
    //init fmod
    fmod = new FmodManager();
    fmod->createStream(RESOURCE_DIR + "bgm.mp3");
    fmod->playSound();

    gui = new GuiManager();
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



    shared_ptr<Shape> cube = make_shared<Shape>();
    cube->loadMesh(RESOURCE_DIR + "cube.obj");
    cube->fitToUnitBox();
    cube->init();
    shapes.push_back(cube);

    //lightPos = vec4(0.0f, 10.0f, 0.0f, 1.0f);
    lightIntensity = 0.6f;



    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    GLSL::checkError(GET_FILE_LINE);



    // Set up the bullet manager and create ground plane and camera.
    bullet = new BulletManager();
    bullet->createPlane("ground", 0, 0, 0);

    bullet->createBox("cam", vec3(0, 0.5, 0), CUBE_HALF_EXTENTS, vec3(1, 1, 1), 1);

    //shared_ptr<Material> material = make_shared<Material>(vec3(0.2f, 0.2f, 0.2f), vec3(0.0f, 0.5f, 0.5f), vec3(1.0f, 0.9f, 0.8f), 200.0f);
    //shared_ptr<BoundingSphere> boundingSphere = make_shared<BoundingSphere>(vec3(0, 10, 0), BUNNY_SPHERE_RADIUS);
    //testObj = make_shared<GameObject>(vec3(0, 10, 0), vec3(1, 0, 0), 0, boundingSphere, shapes.at(0), material);
    //objects.push_back(testObj);
}

void GameManager::createLevel() {
    shared_ptr<Material> building = make_shared<Material>(vec3(0.9f, 0.9f, 0.9f),
            vec3(1.0f, 1.0f, 1.0f),
            vec3(0.0f, 0.0f, 0.0f),
            200.0f);
    shared_ptr<Material> ground = make_shared<Material>(vec3(0.6f, 0.6f, 0.6f),
            vec3(0.7f, 0.7f, 0.7f),
            vec3(0.0f, 0.0f, 0.0f),
            200.0f);
    shared_ptr<Material> greyBox = make_shared<Material>(vec3(0.2f, 0.2f, 0.2f),
            vec3(0.4f, 0.4f, 0.4f),
            vec3(0.0f, 0.0f, 0.0f),
            200.0f);
    shared_ptr<Material> magnetSurface = make_shared<Material>(vec3(0.2f, 0.2f, 0.2f),
            vec3(1.0f, 0.0f, 0.0f),
            vec3(1.0f, 0.9f, 0.8f),
            200.0f);
    vec3 location, direction, scale;

    // Ground Plane
    location = vec3(20, 0, 0);
    direction = vec3(1, 0, 0);
    scale = vec3(100, 0.1, 10);
    shared_ptr<Cuboid> groundPlane = make_shared<Cuboid>(location, direction,
            CUBE_HALF_EXTENTS,
            scale, 0, shapes.at(0),
            ground, false);
    objects.push_back(groundPlane);
    bullet->createBox("groundPlane", location, CUBE_HALF_EXTENTS, scale, 0);

    //
    // Walls
    //
    location = vec3(20, 2, 3);
    direction = vec3(1, 0, 0);
    scale = vec3(70, 50, 1);
    shared_ptr<Cuboid> rightWall = make_shared<Cuboid>(location, direction,
            CUBE_HALF_EXTENTS, scale, 0,
            shapes.at(0), building, false);
    objects.push_back(rightWall);
    bullet->createBox("rightWall", location, CUBE_HALF_EXTENTS, scale, 0);

    location = vec3(20, 2, -3);
    direction = vec3(1, 0, 0);
    scale = vec3(70, 50, 1);
    shared_ptr<Cuboid> leftWall = make_shared<Cuboid>(location, direction,
            CUBE_HALF_EXTENTS, scale, 0,
            shapes.at(0), building, false);
    objects.push_back(leftWall);
    bullet->createBox("leftWall", location, CUBE_HALF_EXTENTS, scale, 0);

    location = vec3(-1, 2, 0);
    direction = vec3(1, 0, 0);
    scale = vec3(1, 50, 5);
    shared_ptr<Cuboid> behindWall = make_shared<Cuboid>(location, direction,
            CUBE_HALF_EXTENTS, scale, 0,
            shapes.at(0), building, false);
    objects.push_back(behindWall);
    bullet->createBox("behindWall", location, CUBE_HALF_EXTENTS, scale, 0);

    location = vec3(53, 2, 0);
    direction = vec3(1, 0, 0);
    scale = vec3(1, 50, 5);
    shared_ptr<Cuboid> endWall = make_shared<Cuboid>(location, direction,
            CUBE_HALF_EXTENTS, scale, 0,
            shapes.at(0), building, false);
    objects.push_back(endWall);
    bullet->createBox("endWall", location, CUBE_HALF_EXTENTS, scale, 0);

    //
    // First Room
    //
    location = vec3(7, 1, 0);
    direction = vec3(1, 0, 0);
    scale = vec3(5, 2.5, 5);
    shared_ptr<Cuboid> firstPlatform = make_shared<Cuboid>(location, direction,
            CUBE_HALF_EXTENTS,
            scale, 0, shapes.at(0),
            building, false);
    objects.push_back(firstPlatform);
    bullet->createBox("firstPlatform", location, CUBE_HALF_EXTENTS, scale, 0);

    location = vec3(3, 0.5, 0);
    direction = vec3(1, 0, 0);
    scale = vec3(1, 1, 1);
    shared_ptr<Cuboid> startingBox = make_shared<Cuboid>(location, direction,
            CUBE_HALF_EXTENTS, scale,
            0, shapes.at(0), greyBox, false);
    objects.push_back(startingBox);
    bullet->createBox("startingBox", location, CUBE_HALF_EXTENTS, scale, 0);

    location = vec3(4.7, 4.2, 3);
    direction = vec3(1, 0, 0);
    scale = vec3(1, 1, 1.5);
    shared_ptr<Cuboid> rightTopPad = make_shared<Cuboid>(location, direction,
            CUBE_HALF_EXTENTS, scale,
            0, shapes.at(0),
            magnetSurface, true);
    objects.push_back(rightTopPad);
    bullet->createMagneticBox("rightTopPad", location, CUBE_HALF_EXTENTS, scale, 0);

    //
    // Jump that you can make
    //
    location = vec3(14, 1, 0);
    direction = vec3(1, 0, 0);
    scale = vec3(5, 2.5, 5);
    shared_ptr<Cuboid> secondPlatform = make_shared<Cuboid>(location, direction,
            CUBE_HALF_EXTENTS, scale,
            0, shapes.at(0),
            building, false);
    objects.push_back(secondPlatform);
    bullet->createBox("secondPlatform", location, CUBE_HALF_EXTENTS, scale, 0);

    location = vec3(10, 0.5, 0);
    direction = vec3(1, 0, 0);
    scale = vec3(1, 1, 1);
    shared_ptr<Cuboid> pitBox = make_shared<Cuboid>(location, direction,
            CUBE_HALF_EXTENTS, scale, 0,
            shapes.at(0), greyBox, false);
    objects.push_back(pitBox);
    bullet->createBox("pitBox", location, CUBE_HALF_EXTENTS, scale, 0);

    //
    // Jump that you can't make but with magnet in pit
    //
    location = vec3(25, 1, 0);
    direction = vec3(1, 0, 0);
    scale = vec3(5, 3, 5);
    shared_ptr<Cuboid> thirdPlatform = make_shared<Cuboid>(location, direction,
            CUBE_HALF_EXTENTS, scale,
            0, shapes.at(0),
            building, false);
    objects.push_back(thirdPlatform);
    bullet->createBox("thirdPlatform", location, CUBE_HALF_EXTENTS, scale, 0);

    location = vec3(20, 0, 0);
    direction = vec3(1, 0, 0);
    scale = vec3(1, 0.2, 1);
    shared_ptr<Cuboid> magnetPadInPit = make_shared<Cuboid>(location, direction,
            CUBE_HALF_EXTENTS,
            scale, 0, shapes.at(0),
            magnetSurface, true);
    objects.push_back(magnetPadInPit);
    bullet->createMagneticBox("magnetPadInPit", location, CUBE_HALF_EXTENTS, scale, 0);

    //
    // Long hallway with pull magnets
    //
    location = vec3(50, 2, 0);
    direction = vec3(1, 0, 0);
    scale = vec3(5, 4, 5);
    shared_ptr<Cuboid> finalPlatform = make_shared<Cuboid>(location, direction,
            CUBE_HALF_EXTENTS,
            scale, 0, shapes.at(0),
            building, false);
    objects.push_back(finalPlatform);
    bullet->createBox("finalPlatform", location, CUBE_HALF_EXTENTS, scale, 0);

    location = vec3(29, 0.5, 0);
    direction = vec3(1, 0, 0);
    scale = vec3(1, 1, 1);
    shared_ptr<Cuboid> hallwayBox = make_shared<Cuboid>(location, direction,
            CUBE_HALF_EXTENTS, scale,
            0, shapes.at(0), greyBox, false);
    objects.push_back(hallwayBox);
    bullet->createBox("hallwayBox", location, CUBE_HALF_EXTENTS, scale, 0);

    location = vec3(30.7, 13, 3);
    direction = vec3(1, 0, 0);
    scale = vec3(1.5, 1.5, 1.5);
    shared_ptr<Cuboid> hallwayFirstPad = make_shared<Cuboid>(location, direction,
            CUBE_HALF_EXTENTS,
            scale, 0, shapes.at(0),
            magnetSurface, true);
    objects.push_back(hallwayFirstPad);
    bullet->createMagneticBox("hallwayFirstPad", location, CUBE_HALF_EXTENTS, scale, 0);

    location = vec3(40.7, 13, -3);
    direction = vec3(1, 0, 0);
    scale = vec3(1.5, 1.5, 1.5);
    shared_ptr<Cuboid> hallwaySecondPad = make_shared<Cuboid>(location, direction,
            CUBE_HALF_EXTENTS,
            scale, 0, shapes.at(0),
            magnetSurface, true);
    objects.push_back(hallwaySecondPad);
    bullet->createMagneticBox("hallwaySecondPad", location, CUBE_HALF_EXTENTS, scale, 0);

    location = vec3(52, 4, 0);
    direction = vec3(1, 0, 0);
    scale = vec3(0.1, 1.5, 1.5);
    shared_ptr<Cuboid> hallwayLastPad = make_shared<Cuboid>(location, direction,
            CUBE_HALF_EXTENTS,
            scale, 0, shapes.at(0),
            magnetSurface, true);
    objects.push_back(hallwayLastPad);
    bullet->createMagneticBox("hallwayLastPad", location, CUBE_HALF_EXTENTS, scale, 0);
}

void GameManager::processInputs() {
    if(gameState == GAME)
        inputManager->processGameInputs(bullet);
}

void GameManager::updateGame(double dt) {
    //bullet->rayTrace(camera->getPosition(), camera->getPosition() + (camera->getDirection() * MAGNET_RANGE));
    resolveMagneticInteractions();

    //step the bullet, update test obj
    bullet->step(dt);

    camera->setPosition(bullet->getBulletObjectState("cam"));


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
        gui->draw();
    }        /* else its in pause menu/game*/
    else {

        // Apply camera transforms
        P->pushMatrix();
        camera->applyProjectionMatrix(P);
        MV->pushMatrix();
        camera->applyViewMatrix(MV);

        // Render objects
        program->bind();
        glUniformMatrix4fv(program->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));
        glUniformMatrix4fv(program->getUniform("MV"), 1, GL_FALSE, value_ptr(MV->topMatrix()));
        lightPos = vec4(camera->getPosition(), 1.0);
        vec4 l = MV->topMatrix() * lightPos;
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
        //cout << "objects draw: " << objectsDrawn << endl;

        program->unbind();


        if (bullet->getDebugFlag())
            bullet->renderDebug(P->topMatrix(), MV->topMatrix());

        MV->popMatrix();
        P->popMatrix();



    }



    //printStringToScreen(0.0f, 0.0f, "+", 0.0f, 0.0f, 0.0f);



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
    shared_ptr<GameObject> obj = RayTrace::rayTrace(startLoc, endLoc, objects);
    if (obj && obj->isMagnetic()) {
        //        cout << to_string(obj->getPosition().x) + " " +
        //                to_string(obj->getPosition().y) + " " +
        //                to_string(obj->getPosition().z) << endl;
        camera->setLookingAtMagnet(true);
        if (Mouse::isLeftMouseButtonPressed()) {
            vec3 dir = normalize(endLoc - startLoc);
            btVector3 bulletDir = btVector3(dir.x, dir.y, dir.z);
            bullet->getBulletObject("cam")->getRigidBody()->setLinearVelocity(bulletDir * MAGNET_STRENGTH);
        } else if (Mouse::isRightMouseButtonPressed()) {
            vec3 dir = normalize(startLoc - endLoc);
            btVector3 bulletDir = btVector3(dir.x, dir.y, dir.z);
            bullet->getBulletObject("cam")->getRigidBody()->setLinearVelocity(bulletDir * MAGNET_STRENGTH);
        }
    } else {
        camera->setLookingAtMagnet(false);
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
