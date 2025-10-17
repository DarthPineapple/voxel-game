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
class Camera;

class Renderer {
public:
    Renderer();
    ~Renderer();

    void init(Window* window);
    void render();
    void cleanup();
    
    Camera* getCamera() { return camera; }
    
    // Debug methods
    Mesh* getTestMesh() { return testMesh; }
    void logMeshInfo() const;
    void logTransformedMeshInfo() const;

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
    
    // Camera
    Camera* camera;
    
    // Uniform buffers for MVP matrix
    VkBuffer* uniformBuffers;
    VkDeviceMemory* uniformBuffersMemory;
    void* uniformBuffersMapped;
    
    // Descriptor pool and sets
    VkDescriptorPool descriptorPool;
    VkDescriptorSet* descriptorSets;
    
    // Rendering state
    size_t currentFrame;
    static const size_t MAX_FRAMES_IN_FLIGHT = 2;
    
    void createUniformBuffers();
    void createDescriptorPool();
    void createDescriptorSets();
    void updateUniformBuffer(uint32_t currentImage);
    void recordCommandBuffer(size_t imageIndex);
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
};

#endif // RENDERER_H