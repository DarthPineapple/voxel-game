#ifndef RENDERER_H
#define RENDERER_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <cstddef>
#include <unordered_map>
#include <tuple>

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

// Hash function for std::tuple<int, int, int>
struct TupleHash {
    std::size_t operator()(const std::tuple<int, int, int>& t) const {
        auto h1 = std::hash<int>{}(std::get<0>(t));
        auto h2 = std::hash<int>{}(std::get<1>(t));
        auto h3 = std::hash<int>{}(std::get<2>(t));
        return h1 ^ (h2 << 1) ^ (h3 << 2);
    }
};

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
    OverlayPipeline* overlayPipeline;
    
    // Mesh for rendering (deprecated - for backwards compatibility)
    Mesh* testMesh;
    
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