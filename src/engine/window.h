#ifndef WINDOW_H
#define WINDOW_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

class Window {
public:
    Window();
    ~Window();

    bool create(const char* title, int width, int height);
    void destroy();
    void processEvents();
    bool shouldClose() const;

    GLFWwindow* getGLFWwindow() const;
    int getWidth() const { return width; }
    int getHeight() const { return height; }

private:
    GLFWwindow* window;
    int width;
    int height;
    const char* title;
};

#endif // WINDOW_H