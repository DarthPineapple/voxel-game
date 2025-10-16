#include "window.h"
#include <GLFW/glfw3.h>
#include <stdexcept>

Window::Window(int width, int height, const char* title) 
    : width(width), height(height), title(title) {
    createWindow();
}

Window::~Window() {
    destroyWindow();
}

void Window::createWindow() {
    if (!glfwInit()) {
        throw std::runtime_error("Failed to initialize GLFW");
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }
}

void Window::destroyWindow() {
    glfwDestroyWindow(window);
    glfwTerminate();
}

void Window::processEvents() {
    glfwPollEvents();
}

GLFWwindow* Window::getGLFWwindow() const {
    return window;
}