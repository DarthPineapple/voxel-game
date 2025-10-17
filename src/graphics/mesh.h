#ifndef MESH_H
#define MESH_H

#include <vulkan/vulkan.h>
#include <vector>

struct Vertex {
    float position[3];
    float normal[3];
    float texCoord[2];
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

private:
    VkDevice device;
    VkPhysicalDevice physicalDevice;
    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
    VkBuffer indexBuffer;
    VkDeviceMemory indexBufferMemory;
    uint32_t indexCount;

    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, 
                     VkMemoryPropertyFlags properties, VkBuffer& buffer, 
                     VkDeviceMemory& bufferMemory);
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
};

#endif // MESH_H
