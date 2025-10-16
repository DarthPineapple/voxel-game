#include "application.h"
#include "window.h"
#include "renderer.h"
#include "chunk_manager.h"

#include <iostream>

Application::Application() : window(nullptr), renderer(nullptr), chunkManager(nullptr) {}

Application::~Application() {
    cleanup();
}

bool Application::init() {
    window = new Window();
    if (!window->create("Voxel Game", 800, 600)) {
        std::cerr << "Failed to create window!" << std::endl;
        return false;
    }

    renderer = new Renderer();
    if (!renderer->init(window)) {
        std::cerr << "Failed to initialize renderer!" << std::endl;
        return false;
    }

    chunkManager = new ChunkManager();
    chunkManager->init();

    return true;
}

void Application::run() {
    while (!window->shouldClose()) {
        window->processEvents();
        chunkManager->updateChunks();
        renderer->render();
    }
}

void Application::cleanup() {
    if (chunkManager) {
        chunkManager->cleanup();
        delete chunkManager;
    }
    if (renderer) {
        renderer->cleanup();
        delete renderer;
    }
    if (window) {
        window->destroy();
        delete window;
    }
}