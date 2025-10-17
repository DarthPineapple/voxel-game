#include "application.h"
#include "window.h"
#include "camera.h"
#include "graphics/renderer.h"
#include "world/chunk_manager.h"

#include <iostream>
#include <GLFW/glfw3.h>

Application::Application() : window(nullptr), renderer(nullptr), chunkManager(nullptr), 
                           isRunning(false), lastTime(0.0) {}

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
        double currentTime = glfwGetTime();
        float deltaTime = static_cast<float>(currentTime - lastTime);
        lastTime = currentTime;
        
        window->processEvents();
        processInput(deltaTime);
        
        Camera* camera = renderer->getCamera();
        if (camera) {
            camera->update(deltaTime);
        }
        
        chunkManager->update();
        renderer->render();
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