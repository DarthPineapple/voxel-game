#ifndef RENDERER_H
#define RENDERER_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

class Window;
class VulkanInstance;
class Device;
class Swapchain;

class Renderer {
public:
    Renderer();
    ~Renderer();

    void init(Window* window);
    void render();
    void cleanup();

private:
    Window* window;
    
    // Vulkan components - now using modular classes
    VulkanInstance* vulkanInstance;
    Device* device;
    Swapchain* swapchain;
};

#endif // RENDERER_H