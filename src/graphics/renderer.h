#ifndef RENDERER_H
#define RENDERER_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <cstddef>
#include <unordered_map>
#include <tuple>
#include "utils/tuple_hash.h"

// Forward declarations
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
class OverlayPipeline;
class Mesh;
class Camera;
class ChunkManager;

class Renderer {
public:
    Renderer();
    ~Renderer();

    void init(Window* window);
    void render();
    void cleanup();
    
    Camera* getCamera() { return camera; }
    
    // Chunk mesh management
    void updateChunkMeshes(ChunkManager* chunkManager);
    
    // Debug methods
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
    OverlayPipeline* overlayPipeline;
    
    // Dynamic chunk meshes
    std::unordered_map<std::tuple<int, int, int>, Mesh*, TupleHash> chunkMeshes;
    
    // Overlay square mesh
    VkBuffer overlayVertexBuffer;
    VkDeviceMemory overlayVertexBufferMemory;
    
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
    double startTime;
    static const size_t MAX_FRAMES_IN_FLIGHT = 2;
    
    void createUniformBuffers();
    void createDescriptorPool();
    void createDescriptorSets();
    void updateUniformBuffer(uint32_t currentImage);
    void recordCommandBuffer(size_t imageIndex);
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    void createOverlayVertexBuffer();
    
    // Helper to create mesh for a chunk
    Mesh* createMeshForChunk(class Chunk* chunk);
};

#endif // RENDERER_H