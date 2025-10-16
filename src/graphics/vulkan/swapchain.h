#ifndef SWAPCHAIN_H
#define SWAPCHAIN_H

#include <vulkan/vulkan.h>
#include <vector>

class Swapchain {
public:
    Swapchain(VkDevice device, VkSurfaceKHR surface);
    ~Swapchain();

    void createSwapchain();
    void cleanup();

    VkSwapchainKHR getSwapchain() const;
    const std::vector<VkImage>& getSwapchainImages() const;
    const VkFormat& getSwapchainImageFormat() const;
    const VkExtent2D& getSwapchainExtent() const;

private:
    VkDevice device;
    VkSurfaceKHR surface;
    VkSwapchainKHR swapchain;
    std::vector<VkImage> swapchainImages;
    VkFormat swapchainImageFormat;
    VkExtent2D swapchainExtent;

    void chooseSwapchainSurfaceFormat();
    void chooseSwapchainPresentMode();
    void chooseSwapchainExtent();
};

#endif // SWAPCHAIN_H