#ifndef COMMAND_POOL_H
#define COMMAND_POOL_H

#include <vulkan/vulkan.h>
#include <vector>

class CommandPool {
public:
    CommandPool(VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
    ~CommandPool();

    void createCommandPool();
    void createCommandBuffers(size_t count);
    void cleanup();

    VkCommandPool getCommandPool() const { return commandPool; }
    const std::vector<VkCommandBuffer>& getCommandBuffers() const { return commandBuffers; }

private:
    VkDevice device;
    VkPhysicalDevice physicalDevice;
    VkSurfaceKHR surface;
    VkCommandPool commandPool;
    std::vector<VkCommandBuffer> commandBuffers;

    int findGraphicsQueueFamily() const;
};

#endif // COMMAND_POOL_H
