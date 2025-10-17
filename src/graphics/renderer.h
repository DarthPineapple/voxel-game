#ifndef RENDERER_H
#define RENDERER_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <cstddef>

class Window;
class VulkanInstance;
class Device;
class Swapchain;
class ImageViews;
class RenderPass;
class Framebuffers;
class CommandPool;
class SyncObjects;
class Pipeline;
class Mesh;

class Renderer {
public:
    Renderer();
    ~Renderer();

    void init(Window* window);
    void render();
    void cleanup();

private:
    Window* window;
    
    // Vulkan components - using modular classes
    VulkanInstance* vulkanInstance;
    Device* device;
    Swapchain* swapchain;
    ImageViews* imageViews;
    RenderPass* renderPass;
    Framebuffers* framebuffers;
    CommandPool* commandPool;
    SyncObjects* syncObjects;
    Pipeline* pipeline;
    
    // Mesh for rendering
    Mesh* testMesh;
    
    // Rendering state
    size_t currentFrame;
    static const size_t MAX_FRAMES_IN_FLIGHT = 2;
    
    void recordCommandBuffer(size_t imageIndex);
};

#endif // RENDERER_H