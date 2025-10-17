#ifndef VULKAN_INSTANCE_H
#define VULKAN_INSTANCE_H

#include <vulkan/vulkan.h>

class Window;

class VulkanInstance {
public:
    VulkanInstance();
    ~VulkanInstance();

    void createInstance();
    void createSurface(Window* window);
    void cleanup();

    VkInstance getInstance() const { return instance; }
    VkSurfaceKHR getSurface() const { return surface; }

private:
    VkInstance instance;
    VkSurfaceKHR surface;
};

#endif // VULKAN_INSTANCE_H
