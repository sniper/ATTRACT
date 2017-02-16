//
//  GameManager.cpp
//  Lab1
//
//  Created by Cody Rhoads on 1/17/17.
//
//

#include <algorithm>
#include <iostream>
#include <fstream>
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

#define MAX_NUM_OBJECTS 15
#define GRID_SIZE 32
#define OBJ_SPAWN_INTERVAL 2
#define BUNNY_SPHERE_RADIUS 0.5f

#define X_AXIS 0
#define Y_AXIS 1
#define Z_AXIS 2

using namespace std;
using namespace glm;

GameManager::GameManager(GLFWwindow *window, const string &resourceDir, const string &levelDir, const string &level) :
window(window),
RESOURCE_DIR(resourceDir),
LEVEL_DIR(levelDir),
level(level)
{
    objIntervalCounter = 0.0f;
    numObjCollected = 0;
    gameWon = false;

    currentObject = 0;
    currentAxis = X_AXIS;
    objectPlacement = false;
    setSpawn = false;
    setCollectable = false;

    // Set vsync.
    glfwSwapInterval(1);
    // Set keyboard callback.
    glfwSetKeyCallback(window, Keyboard::key_callback);
    // Sets cursor movement to unlimited.
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // Set cursor position callback.
    glfwSetCursorPosCallback(window, Mouse::cursor_position_callback);
    // Set cursor button callback.
    glfwSetMouseButtonCallback(window, Mouse::mouse_button_callback);
    // Set the window resize call back.
    glfwSetFramebufferSizeCallback(window, resize_callback);
    // Create the camera for the scene.
    camera = make_shared<Camera>(GRID_SIZE);
    // Set up the manager for keyboard and mouse input.
    inputManager = make_shared<InputManager>(camera);
    // Initialize the scene.
    initScene();

    if (!level.empty()) {
        importLevel(level);
    }
}

GameManager::~GameManager()
{
    
}

bool toBool(string s) {
    return s != "0";
}

shared_ptr<GameObject> GameManager::parseObject(string objectString) {
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

    return createObject(pos, scale, rot, magnetic, deadly, playerSpawn, collectable);
}

void GameManager::importLevel(string level)
{
    string line;
    ifstream file;
    file.open(level);
    if (file.is_open()) {
        if (getline(file, line)) {
            playerSpawn = parseObject(line);
        }
        if (getline(file, line)) {
            spaceshipPart = parseObject(line);
        }
        while (getline(file, line)) {
            objects.push_back(parseObject(line));
        }
        cout << "Level '" << level << "' successfully imported." << endl;
        file.close();
    } else {
        cout << "Unable to open level '" << level << "'" << endl;
    }
}

void GameManager::initScene()
{
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
    program->addUniform("selected");
    
    shared_ptr<Shape> cube = make_shared<Shape>();
    cube->loadMesh(RESOURCE_DIR + "cube.obj");
    cube->fitToUnitBox();
    cube->init();
    shapes.push_back(cube);
    
    lightPos = vec4(0.0f, 10.0f, 0.0f, 1.0f);
    lightIntensity = 0.8f;
    
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
    grassIndCount = (int)grassIndBuf.size();
    
    GLuint tmp[3];
    glGenBuffers(3, tmp);
    grassBufIDs["bPos"] = tmp[0];
    grassBufIDs["bTex"] = tmp[1];
    grassBufIDs["bInd"] = tmp[2];
    
    glBindBuffer(GL_ARRAY_BUFFER, grassBufIDs["bPos"]);
    glBufferData(GL_ARRAY_BUFFER, grassPosBuf.size()*sizeof(float), &grassPosBuf[0], GL_STATIC_DRAW);
    
    glBindBuffer(GL_ARRAY_BUFFER, grassBufIDs["bTex"]);
    glBufferData(GL_ARRAY_BUFFER, grassTexBuf.size()*sizeof(float), &grassTexBuf[0], GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, grassBufIDs["bInd"]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, grassIndBuf.size()*sizeof(unsigned int), &grassIndBuf[0], GL_STATIC_DRAW);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    GLSL::checkError(GET_FILE_LINE);

    tempObject = createObject(false, false, false, false);
    playerSpawn = createObject(false, false, true, false);
    spaceshipPart = createObject(false, false, false, true);
}

void GameManager::processInputs()
{
    vector<char> objectKeys = inputManager->processInputs();

    if (find(objectKeys.begin(), objectKeys.end(), 'p') != objectKeys.end()) {
        objectPlacement = !objectPlacement;
    }

    if (find(objectKeys.begin(), objectKeys.end(), 'j') != objectKeys.end()) {
        if (objects.size() > 0) {
            if (currentObject == 0) {
                currentObject = objects.size() - 1;
            } else {
                currentObject--;
            }
        }
        objectPlacement = false;
    }

    if (find(objectKeys.begin(), objectKeys.end(), 'k') != objectKeys.end()) {
        currentObject++;
        if (currentObject >= objects.size()) {
            currentObject = 0;
        }
        objectPlacement = false;
    }

    if (find(objectKeys.begin(), objectKeys.end(), '1') != objectKeys.end()) {
        setSpawn = true;
        setCollectable = false;
        objectPlacement = false;
    }

    if (find(objectKeys.begin(), objectKeys.end(), '2') != objectKeys.end()) {
        setCollectable = true;
        setSpawn = false;
        objectPlacement = false;
    }

    shared_ptr<GameObject> obj;
    if (objectPlacement) {
        obj = tempObject;
    } else if (objects.size() > 0) {
        obj = objects.at(currentObject);
    } else {
        return;
    }

    if (find(objectKeys.begin(), objectKeys.end(), 'x') != objectKeys.end()) {
        currentAxis = X_AXIS;
    }
    if (find(objectKeys.begin(), objectKeys.end(), 'y') != objectKeys.end()) {
        currentAxis = Y_AXIS;
    }
    if (find(objectKeys.begin(), objectKeys.end(), 'z') != objectKeys.end()) {
        currentAxis = Z_AXIS;
    }

    float modifier;
    if (find(objectKeys.begin(), objectKeys.end(), '^') != objectKeys.end()) {
        modifier = 0.1;
    } else {
        modifier = 1.0;
    }
    float modSpeed = 0.1 * modifier;

    // rotate
    vec3 rotate = obj->getRotation();
    if (find(objectKeys.begin(), objectKeys.end(), 'L') != objectKeys.end()) {
        rotate[currentAxis] -= modSpeed;
        obj->setRotation(rotate);
    }
    if (find(objectKeys.begin(), objectKeys.end(), 'R') != objectKeys.end()) {
        rotate[currentAxis] += modSpeed;
        obj->setRotation(rotate);
    }

    // translate
    vec3 position = obj->getPosition();
    if (find(objectKeys.begin(), objectKeys.end(), 'U') != objectKeys.end()) {
        position[currentAxis] += modSpeed;
        obj->setPosition(position);
    }
    if (find(objectKeys.begin(), objectKeys.end(), 'D') != objectKeys.end()) {
        position[currentAxis] -= modSpeed;
        obj->setPosition(position);
    }

    // scale
    vec3 scale = obj->getScale();
    if (find(objectKeys.begin(), objectKeys.end(), 'q') != objectKeys.end()) {
        scale[currentAxis] -= modSpeed;
        obj->setScale(scale);
    }
    if (find(objectKeys.begin(), objectKeys.end(), 'e') != objectKeys.end()) {
        scale[currentAxis] += modSpeed;
        obj->setScale(scale);
    }

    if (find(objectKeys.begin(), objectKeys.end(), '3') != objectKeys.end()) {
        obj->setMagnetic(!obj->getMagnetic());
    }
    if (find(objectKeys.begin(), objectKeys.end(), '4') != objectKeys.end()) {
        obj->setDeadly(!obj->getDeadly());
    }

    if (find(objectKeys.begin(), objectKeys.end(), 'X') != objectKeys.end()) {
        objects.erase(objects.begin() + currentObject);
        if (currentObject >= objects.size()) {
            currentObject--;
        }
    }

    if (find(objectKeys.begin(), objectKeys.end(), 'E') != objectKeys.end()) {
        if (playerSpawn == NULL || spaceshipPart == NULL) {
            cout << "Place a player spawn and spaceship part before saving." << endl;
            return;
        }
        char str[80];
        cout << "Enter a file name: ";
        scanf("%73s", str);
        strcat(str, ".level");
        ofstream file;
        file.open(LEVEL_DIR + str);
        file << playerSpawn->toString() << endl << spaceshipPart->toString() << endl;
        for (unsigned int i = 0; i < objects.size(); i++) {
            file << objects.at(i)->toString() << endl;
        }
        file.close();
        cout << "Level '" << LEVEL_DIR + str << "' successfully exported." << endl;
    }
}

void GameManager::renderGame(int fps)
{
    // Clear framebuffer.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    if(Keyboard::isPressed(GLFW_KEY_C)) {
        glEnable(GL_CULL_FACE);
    }
    else {
        glDisable(GL_CULL_FACE);
    }
    
    if(Keyboard::isPressed(GLFW_KEY_L)) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    
    // Get current frame buffer size.
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    camera->setAspect((float)width/(float)height);
    
    // Matrix stacks
    auto P = make_shared<MatrixStack>();
    auto MV = make_shared<MatrixStack>();
    
    // Apply camera transforms
    P->pushMatrix();
    camera->applyProjectionMatrix(P);
    MV->pushMatrix();
    camera->applyViewMatrix(MV);
    
    // Render ground
    ground->bind();
    grass->bind(ground->getUniform("grassTexture"));
    glUniformMatrix4fv(ground->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));
    glUniformMatrix4fv(ground->getUniform("MV"), 1, GL_FALSE, value_ptr(MV->topMatrix()));
    glEnableVertexAttribArray(ground->getAttribute("aPos"));
    glBindBuffer(GL_ARRAY_BUFFER, grassBufIDs["bPos"]);
    glVertexAttribPointer(ground->getAttribute("aPos"), 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
    glEnableVertexAttribArray(ground->getAttribute("aTex"));
    glBindBuffer(GL_ARRAY_BUFFER, grassBufIDs["bTex"]);
    glVertexAttribPointer(ground->getAttribute("aTex"), 2, GL_FLOAT, GL_FALSE, 0, (void *)0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, grassBufIDs["bInd"]);
    glDrawElements(GL_TRIANGLES, grassIndCount, GL_UNSIGNED_INT, (void *)0);
    grass->unbind();
    ground->unbind();
 
    // Render objects
    if (Mouse::wasLeftMouseClicked() && (objectPlacement || setSpawn || setCollectable)) {
        if (objectPlacement) {
            objects.push_back(tempObject);
            tempObject = createObject(false, false, false, false);
            if (currentObject > 0) {
                currentObject++;
            }
        } else if (setSpawn) {
            setSpawn = false;
        } else if (setCollectable) {
            setCollectable = false;
        }
        Mouse::clearClick(GLFW_MOUSE_BUTTON_LEFT);
    }
    program->bind();
    glUniformMatrix4fv(program->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));
    glUniformMatrix4fv(program->getUniform("MV"), 1, GL_FALSE, value_ptr(MV->topMatrix()));
    vec4 l = MV->topMatrix() * lightPos;
    glUniform4f(program->getUniform("lightPos"), l[0] , l[1], l[2], l[3]);
    glUniform1f(program->getUniform("lightIntensity"), lightIntensity);
    if (objectPlacement) {
        tempObject->setPosition(camera->getPosition() + (2.0f * camera->getForward()));
        tempObject->draw(program);
    }
    if (setSpawn) {
        playerSpawn->setPosition(camera->getPosition() + (2.0f * camera->getForward()));
    }
    playerSpawn->draw(program);
    if (setCollectable) {
        spaceshipPart->setPosition(camera->getPosition() + (2.0f * camera->getForward()));
    }
    spaceshipPart->draw(program);

    if (objects.size() > 0 && !objectPlacement && !setSpawn && !setCollectable) {
        objects.at(currentObject)->setSelected(true);
    }

    for (unsigned int i = 0; i < objects.size(); i++) {
        objects.at(i)->draw(program);
    }
    program->unbind();

    if (objects.size() > 0) {
        objects.at(currentObject)->setSelected(false);
    }
    
    // Render sun
    sunProg->bind();
    MV->pushMatrix();
    MV->translate(vec3(0.0f, 10.0f, 0.0f));
    glUniformMatrix4fv(sunProg->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));
    glUniformMatrix4fv(sunProg->getUniform("MV"), 1, GL_FALSE, value_ptr(MV->topMatrix()));
    sun->draw(sunProg);
    MV->popMatrix();
    sunProg->unbind();

    //
    // stb_easy_font.h is used for printing fonts to the screen.
    //
    printStringToScreen(0, 0, "+", 0, 0, 0); 
    printStringToScreen(60.0f, -95.0f, to_string(fps) + " FPS", 0.0f, 0.0f, 0.0f);

    if (objects.size() > 0 || objectPlacement || setSpawn || setCollectable) {
        shared_ptr<GameObject> obj;
        if (objectPlacement) {
            obj = tempObject;
        } else if (setSpawn) {
            obj = playerSpawn;
        } else if (setCollectable) {
            obj = spaceshipPart;
        } else {
            obj = objects.at(currentObject);
        }
        vec3 pos = obj->getPosition();
        vec3 scale = obj->getScale();
        vec3 rotation = obj->getRotation();
        printStringToScreen(-95.0f, 70.0f, "X: " + to_string(pos.x) + " " + to_string(scale.x) + " " + to_string(rotation.x), (currentAxis == X_AXIS ? 1.0f : 0.0f), 0.0f, 0.0f);
        printStringToScreen(-95.0f, 80.0f, "Y: " + to_string(pos.y) + " " + to_string(scale.y) + " " + to_string(rotation.y), (currentAxis == Y_AXIS ? 1.0f : 0.0f), 0.0f, 0.0f);
        printStringToScreen(-95.0f, 90.0f, "Z: " + to_string(pos.z) + " " + to_string(scale.z) + " " + to_string(rotation.z), (currentAxis == Z_AXIS ? 1.0f : 0.0f), 0.0f, 0.0f);

        if (objectPlacement) {
            printStringToScreen(-95.0f, -95.0f, "Object: new (total: " + to_string(objects.size()) + ")", 0.0f, 0.0f, 0.0f);
        } else if (setSpawn) {
            printStringToScreen(-95.0f, -95.0f, "Object: Spawn", 0.0f, 0.0f, 0.0f);
        } else if (setCollectable) {
            printStringToScreen(-95.0f, -95.0f, "Object: Spaceship Part", 0.0f, 0.0f, 0.0f);
        } else {
            printStringToScreen(-95.0f, -95.0f, "Object: " + to_string(currentObject + 1) + " / " + to_string(objects.size()), 0.0f, 0.0f, 0.0f);
        }
    }

    MV->popMatrix();
    P->popMatrix();
    
    GLSL::checkError(GET_FILE_LINE);
}

// If the window is resized, capture the new size and reset the viewport
void GameManager::resize_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

shared_ptr<GameObject> GameManager::createObject(vec3 position, vec3 scale, vec3 rotation, bool magnetic, bool deadly, bool spawnPoint, bool collectable)
{
    return make_shared<GameObject>(position, scale, rotation, shapes.at(0), magnetic, deadly, spawnPoint, collectable);
}

shared_ptr<GameObject> GameManager::createObject(bool magnetic, bool deadly, bool spawnPoint, bool collectable)
{
    return createObject(vec3(0.0f, 0.0f, 0.0f), vec3(1.0f, 1.0f, 1.0f), vec3(0.0f, 0.0f, 0.0f), magnetic, deadly, spawnPoint, collectable);
}

void GameManager::printStringToScreen(float x, float y, const string &text, float r, float g, float b)
{
    static char buffer[99999]; // ~500 chars
    int num_quads;
    
    char *toScreen = new char[text.size() + 1];
    strcpy(toScreen, text.c_str());
    
    // With this disabled, text will always be printed to the screen, even if
    // an object is "in front" of it.
    glDisable(GL_DEPTH_TEST);
    
    //
    glPushMatrix();
    num_quads = stb_easy_font_print(x, y, toScreen, NULL, buffer, sizeof(buffer));
    glScalef(0.01f, -0.01f, 1.0f);
    glColor3f(r,g,b);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 16, buffer);
    glDrawArrays(GL_QUADS, 0, num_quads*4);
    glDisableClientState(GL_VERTEX_ARRAY);
    glPopMatrix();
    
    // Re-enable depth test for further render calls.
    glEnable(GL_DEPTH_TEST);
    
    delete[] toScreen;
}

// Returns a random float between the provided low and high floats.
float GameManager::randFloat(float l, float h)
{
    float r = rand() / (float)RAND_MAX;
    return (1.0f - r) * l + r * h;
}
