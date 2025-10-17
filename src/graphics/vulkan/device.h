#ifndef DEVICE_H
#define DEVICE_H

#include <vulkan/vulkan.h>

class Device {
public:
    Device();
    ~Device();

    void pickPhysicalDevice(VkInstance instance, VkSurfaceKHR surface);
    void createLogicalDevice(VkSurfaceKHR surface);
    void cleanup();

    VkDevice getDevice() const { return device; }
    VkPhysicalDevice getPhysicalDevice() const { return physicalDevice; }
    VkQueue getGraphicsQueue() const { return graphicsQueue; }
    VkQueue getPresentQueue() const { return presentQueue; }

private:
    VkDevice device;
    VkPhysicalDevice physicalDevice;
    VkQueue graphicsQueue;
    VkQueue presentQueue;

    bool deviceSupportsExtensions(VkPhysicalDevice dev) const;
    bool findQueueFamilies(VkPhysicalDevice dev, VkSurfaceKHR surface, int& gfx, int& present) const;
    bool swapchainAdequate(VkPhysicalDevice dev, VkSurfaceKHR surface) const;
};

#endif // DEVICE_H