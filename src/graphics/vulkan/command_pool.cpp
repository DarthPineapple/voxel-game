#include "command_pool.h"
#include <stdexcept>
#include <vector>

CommandPool::CommandPool(VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface)
    : device(device), physicalDevice(physicalDevice), surface(surface), commandPool(VK_NULL_HANDLE) {
}

CommandPool::~CommandPool() {
    cleanup();
}

void CommandPool::createCommandPool() {
    int graphicsFamily = findGraphicsQueueFamily();
    if (graphicsFamily < 0) {
        throw std::runtime_error("Failed to find graphics queue family!");
    }

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = static_cast<uint32_t>(graphicsFamily);

    if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create command pool!");
    }
}

void CommandPool::createCommandBuffers(size_t count) {
    commandBuffers.resize(count);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

    if (vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate command buffers!");
    }
}

void CommandPool::cleanup() {
    if (commandPool != VK_NULL_HANDLE) {
        vkDestroyCommandPool(device, commandPool, nullptr);
        commandPool = VK_NULL_HANDLE;
    }
    commandBuffers.clear();
}

int CommandPool::findGraphicsQueueFamily() const {
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

    for (size_t i = 0; i < queueFamilies.size(); i++) {
        if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            return static_cast<int>(i);
        }
    }
    return -1;
}
