#include "application.h"
#include "window.h"
#include "camera.h"
#include "graphics/renderer.h"
#include "world/chunk_manager.h"

#include <iostream>
#include <unordered_map>
#include <GLFW/glfw3.h>

Application::Application() : window(nullptr), renderer(nullptr), chunkManager(nullptr), 
                           isRunning(false), lastTime(0.0), debugMode(false), 
                           debugStepMode(false), debugStepRequested(false),
                           prevF1KeyState(false), prevF2KeyState(false) {}

Application::~Application() {
    cleanup();
}

void Application::init() {
    window = new Window();
    if (!window->create("Voxel Game", 800, 600)) {
        std::cerr << "Failed to create window!" << std::endl;
        throw std::runtime_error("Failed to create window");
    }

    renderer = new Renderer();
    renderer->init(window);

    chunkManager = new ChunkManager();
    chunkManager->init();

    isRunning = true;
    lastTime = glfwGetTime();
}

void Application::run() {
    while (!window->shouldClose() && isRunning) {
        double frameStart = glfwGetTime();
        double currentTime = frameStart;
        float deltaTime = static_cast<float>(currentTime - lastTime);
        lastTime = currentTime;
        
        window->processEvents();
        processInput(deltaTime);
        
        // Debug mode: frame-by-frame stepping
        if (debugMode && debugStepMode) {
            if (!debugStepRequested) {
                // Wait for step request (F2 key)
                continue;
            }
            debugStepRequested = false; // Consume the step request
        }
        
        Camera* camera = renderer->getCamera();
        if (camera) {
            camera->update(deltaTime);
        }
        
        chunkManager->update();
        renderer->render();

        // Frame logger: output render time, camera pitch/yaw, and position
        double frameEnd = glfwGetTime();
        double renderTimeMs = (frameEnd - frameStart) * 1000.0;
        if (debugMode) {
            std::cout << "\n========== DEBUG FRAME INFO ==========\n";
            std::cout << "[Frame] Render time: " << renderTimeMs << " ms\n";
            if (camera) {
                std::cout << "[Camera] Position: (" << camera->getPosX() << ", " 
                          << camera->getPosY() << ", " << camera->getPosZ() << ")\n"
                          << "[Camera] Yaw: " << camera->getYaw() << " Pitch: " << camera->getPitch() << "\n";
            }
            logDebugInfo();
            std::cout << "======================================\n" << std::endl;
        } else if (camera) {
            std::cout << "[Frame] Render time: " << renderTimeMs << " ms"
                      << " | Camera Pos: (" << camera->getPosX() << ", " << camera->getPosY() << ", " << camera->getPosZ() << ")"
                      << " | Yaw: " << camera->getYaw() << " Pitch: " << camera->getPitch() << std::endl;
        } else {
            std::cout << "[Frame] Render time: " << renderTimeMs << " ms" << std::endl;
        }
    }
}

void Application::cleanup() {
    if (chunkManager) {
        chunkManager->cleanup();
        delete chunkManager;
        chunkManager = nullptr;
    }
    if (renderer) {
        renderer->cleanup();
        delete renderer;
        renderer = nullptr;
    }
    if (window) {
        window->destroy();
        delete window;
        window = nullptr;
    }
}

void Application::processInput(float deltaTime) {
    Camera* camera = renderer->getCamera();
    if (!camera) return;

    // Debug mode toggle (F1 key)
    bool f1Pressed = window->isKeyPressed(GLFW_KEY_F1);
    if (f1Pressed && !prevF1KeyState) {
        debugMode = !debugMode;
        if (debugMode) {
            std::cout << "\n*** DEBUG MODE ENABLED ***" << std::endl;
            std::cout << "Press F2 to enable frame-by-frame stepping" << std::endl;
            std::cout << "Press F1 to disable debug mode\n" << std::endl;
        } else {
            std::cout << "\n*** DEBUG MODE DISABLED ***\n" << std::endl;
            debugStepMode = false;
        }
    }
    prevF1KeyState = f1Pressed;

    // Frame stepping toggle (F2 key) - only in debug mode
    bool f2Pressed = window->isKeyPressed(GLFW_KEY_F2);
    if (debugMode && f2Pressed && !prevF2KeyState) {
        if (debugStepMode) {
            // F2 advances to next frame when in step mode
            debugStepRequested = true;
            std::cout << "[Debug] Advancing to next frame..." << std::endl;
        } else {
            // First F2 press enables step mode
            debugStepMode = true;
            debugStepRequested = true;
            std::cout << "\n*** FRAME-BY-FRAME MODE ENABLED ***" << std::endl;
            std::cout << "Press F2 to advance to next frame" << std::endl;
            std::cout << "Press F1 to disable debug mode\n" << std::endl;
        }
    }
    prevF2KeyState = f2Pressed;

    // Key logging: log on press/release edges for tracked keys
    static std::unordered_map<int, bool> prev;
    static const std::pair<int, const char*> keys[] = {
        {GLFW_KEY_W, "W"}, {GLFW_KEY_A, "A"}, {GLFW_KEY_S, "S"}, {GLFW_KEY_D, "D"},
        {GLFW_KEY_SPACE, "SPACE"}, {GLFW_KEY_LEFT_SHIFT, "LEFT_SHIFT"},
        {GLFW_KEY_LEFT, "LEFT"}, {GLFW_KEY_RIGHT, "RIGHT"},
        {GLFW_KEY_UP, "UP"}, {GLFW_KEY_DOWN, "DOWN"}
    };
    for (size_t i = 0; i < sizeof(keys) / sizeof(keys[0]); ++i) {
        int code = keys[i].first;
        const char* name = keys[i].second;
        bool isDown = window->isKeyPressed(code);
        bool wasDown = prev.count(code) ? prev[code] : false;
        if (isDown != wasDown) {
            std::cout << "Key " << name << (isDown ? " pressed" : " released") << std::endl;
            prev[code] = isDown;
        }
    }

    // Movement input (WASD + Space/Shift for up/down)
    float forward = 0.0f;
    float right = 0.0f;
    float up = 0.0f;
    
    if (window->isKeyPressed(GLFW_KEY_W)) forward += 1.0f;
    if (window->isKeyPressed(GLFW_KEY_S)) forward -= 1.0f;
    if (window->isKeyPressed(GLFW_KEY_D)) right += 1.0f;
    if (window->isKeyPressed(GLFW_KEY_A)) right -= 1.0f;
    if (window->isKeyPressed(GLFW_KEY_SPACE)) up += 1.0f;
    if (window->isKeyPressed(GLFW_KEY_LEFT_SHIFT)) up -= 1.0f;
    camera->setMovementInput(forward, right, up);

    // Rotation input (Arrow keys)
    float yaw = 0.0f;
    float pitch = 0.0f;
    
    if (window->isKeyPressed(GLFW_KEY_LEFT)) yaw += 1.0f;
    if (window->isKeyPressed(GLFW_KEY_RIGHT)) yaw -= 1.0f;
    if (window->isKeyPressed(GLFW_KEY_UP)) pitch += 1.0f;
    if (window->isKeyPressed(GLFW_KEY_DOWN)) pitch -= 1.0f;
    camera->setRotationInput(yaw, pitch);
}

void Application::logDebugInfo() {
    if (!renderer) return;
    
    // Log mesh information
    renderer->logMeshInfo();
    
    // Log transformed mesh information
    renderer->logTransformedMeshInfo();
}