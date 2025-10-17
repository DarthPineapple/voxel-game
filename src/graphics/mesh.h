#ifndef MESH_H
#define MESH_H

#include <vulkan/vulkan.h>
#include <vector>

// Vertex structure for voxel mesh rendering
// Memory layout matches shader expectations:
//   location 0: vec3 position (12 bytes)
//   location 1: vec3 normal (12 bytes)
//   location 2: vec2 texCoord (8 bytes)
// Total size: 32 bytes per vertex
struct Vertex {
    float position[3];  // Vertex position in world space
    float normal[3];    // Surface normal for lighting
    float texCoord[2];  // Texture coordinates (UV mapping)
};

class Mesh {
public:
    Mesh(VkDevice device, VkPhysicalDevice physicalDevice);
    ~Mesh();

    void createVertexBuffer(const std::vector<Vertex>& vertices);
    void createIndexBuffer(const std::vector<uint32_t>& indices);
    void cleanup();

    VkBuffer getVertexBuffer() const { return vertexBuffer; }
    VkBuffer getIndexBuffer() const { return indexBuffer; }
    uint32_t getIndexCount() const { return indexCount; }
    uint32_t getVertexCount() const { return vertexCount; }
    
    // Debug methods
    const std::vector<Vertex>& getVertices() const { return vertices; }

private:
    VkDevice device;
    VkPhysicalDevice physicalDevice;
    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
    VkBuffer indexBuffer;
    VkDeviceMemory indexBufferMemory;
    uint32_t indexCount;
    uint32_t vertexCount;
    
    // Store a copy of vertices for debug purposes
    std::vector<Vertex> vertices;

    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, 
                     VkMemoryPropertyFlags properties, VkBuffer& buffer, 
                     VkDeviceMemory& bufferMemory);
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
};

#endif // MESH_H
