#include "application.h"
#include "window.h"
#include "graphics/renderer.h"
#include "world/chunk_manager.h"

#include <iostream>

Application::Application() : window(nullptr), renderer(nullptr), chunkManager(nullptr), isRunning(false) {}

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
}

void Application::run() {
    while (!window->shouldClose() && isRunning) {
        window->processEvents();
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