#ifndef SWAPCHAIN_H
#define SWAPCHAIN_H

#include <vulkan/vulkan.h>
#include <vector>

class Window;

class Swapchain {
public:
    Swapchain(VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
    ~Swapchain();

    void createSwapchain(Window* window);
    void cleanup();

    VkSwapchainKHR getSwapchain() const;
    const std::vector<VkImage>& getSwapchainImages() const;
    const VkFormat& getSwapchainImageFormat() const;
    const VkExtent2D& getSwapchainExtent() const;

private:
    VkDevice device;
    VkPhysicalDevice physicalDevice;
    VkSurfaceKHR surface;
    VkSwapchainKHR swapchain;
    std::vector<VkImage> swapchainImages;
    VkFormat swapchainImageFormat;
    VkExtent2D swapchainExtent;

    VkSurfaceFormatKHR chooseSwapchainSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats) const;
    VkPresentModeKHR chooseSwapchainPresentMode(const std::vector<VkPresentModeKHR>& presentModes) const;
    VkExtent2D chooseSwapchainExtent(const VkSurfaceCapabilitiesKHR& capabilities, Window* window) const;
};

#endif // SWAPCHAIN_H