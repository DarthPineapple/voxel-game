#include "window.h"
#include <stdexcept>
#include <iostream>

Window::Window() 
    : window(nullptr), width(800), height(600), title("Voxel Game") {
}

Window::~Window() {
    destroy();
}

bool Window::create(const char* title, int width, int height) {
    this->title = title;
    this->width = width;
    this->height = height;

    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    
    window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }

    return true;
}

void Window::destroy() {
    if (window) {
        glfwDestroyWindow(window);
        window = nullptr;
    }
    glfwTerminate();
}

void Window::processEvents() {
    glfwPollEvents();
}

bool Window::shouldClose() const {
    return window && glfwWindowShouldClose(window);
}

GLFWwindow* Window::getGLFWwindow() const {
    return window;
}

bool Window::isKeyPressed(int key) const {
    if (!window) return false;
    return glfwGetKey(window, key) == GLFW_PRESS;
}