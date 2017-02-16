#include <cassert>
#include <cstring>
#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <chrono>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "GLSL.h"
#include "GameManager.hpp"

#define IDEAL_DT 0.1
#define FPS_UPDATE_INTERVAL 0.5

using namespace std;
using namespace chrono;

// This function is called when a GLFW error occurs

static void error_callback(int error, const char *description) {
    cerr << description << endl;
}

int main(int argc, char **argv) {
    GLFWwindow *window; // Main application window
    shared_ptr<GameManager> gameManager; // Manages all aspects of the game.
    string RESOURCE_DIR = "./"; // Where the resources are loaded from

    time_point<system_clock> curTime, oldTime; // Variables for time.
    double elapsedTime = 0, dt = 0;
    int fps = 60;
    float fpsIntervalCounter = 0.0f;
    State gameState;

    bool firstTimeThroughGameLoop = true;

    if (argc < 2) {
        cout << "Please specify the resource directory" << endl;
        return 0;
    }
    RESOURCE_DIR = argv[1] + string("/");

    // Set error callback.
    glfwSetErrorCallback(error_callback);
    // Initialize the library.
    if (!glfwInit()) {
        return -1;
    }
    // Create a windowed mode window and its OpenGL context.
    window = glfwCreateWindow(640, 480, "ATTRACT", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    // Make the window's context current.
    glfwMakeContextCurrent(window);
    // Initialize GLEW.
    glewExperimental = true;
    if (glewInit() != GLEW_OK) {
        cerr << "Failed to initialize GLEW" << endl;
        return -1;
    }
    glGetError(); // A bug in glewInit() causes an error that we can safely ignore.
    cout << "OpenGL version: " << glGetString(GL_VERSION) << endl;
    cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
    GLSL::checkVersion();

    gameManager = make_shared<GameManager>(window, RESOURCE_DIR);

    // Loop until the user closes the window.
    while (!glfwWindowShouldClose(window)) {
        curTime = system_clock::now();

        // We don't compute the elapsed time for the first game loop, since there
        // is no previous loop to reference.
        if (!firstTimeThroughGameLoop) {
            // Find the elapsed time in milliseconds between the beginnings of the last and current loop iteration.
            // Elapsed time is divided by 1000 to convert it to seconds.
            elapsedTime = duration_cast<milliseconds>(curTime - oldTime).count() / 1000.0;
        } else {
            firstTimeThroughGameLoop = false;
        }

        // Poll inputs
        gameState = gameManager->processInputs();
        if (gameState == GAME) {
            while (elapsedTime > 0) {
                dt = glm::min(elapsedTime, IDEAL_DT);
                // Simulate movement/physics
                gameManager->updateGame(dt);
                elapsedTime -= dt;
                if (gameManager->getState() != GAME) {
                    break;
                }
            }
        }

        // Render scene.
        gameManager->renderGame(fps);
        // Swap front and back buffers.
        glfwSwapBuffers(window);
        // Poll for and process events.
        glfwPollEvents();

        oldTime = curTime;

        // Keep FPS updates to a reasonable speed. Don't want to spam updates so fast
        // that you can barely read it.
        fpsIntervalCounter += dt;
        if (fpsIntervalCounter > FPS_UPDATE_INTERVAL) {
            fps = (int) (1 / dt);
            fpsIntervalCounter = 0.0f;
        }
    }
    // Quit program.
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
