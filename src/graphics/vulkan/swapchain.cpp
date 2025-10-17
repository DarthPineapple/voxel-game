#include "swapchain.h"
#include <stdexcept>

Swapchain::Swapchain(VkDevice device, VkSurfaceKHR surface)
    : device(device), surface(surface), swapchain(VK_NULL_HANDLE), 
      swapchainImageFormat(VK_FORMAT_UNDEFINED), swapchainExtent{0, 0} {
}

Swapchain::~Swapchain() {
    cleanup();
}

void Swapchain::createSwapchain() {
    // Stub implementation - not used in simplified renderer
}

void Swapchain::cleanup() {
    if (swapchain != VK_NULL_HANDLE) {
        vkDestroySwapchainKHR(device, swapchain, nullptr);
        swapchain = VK_NULL_HANDLE;
    }
}

VkSwapchainKHR Swapchain::getSwapchain() const {
    return swapchain;
}

const std::vector<VkImage>& Swapchain::getSwapchainImages() const {
    return swapchainImages;
}

const VkFormat& Swapchain::getSwapchainImageFormat() const {
    return swapchainImageFormat;
}

const VkExtent2D& Swapchain::getSwapchainExtent() const {
    return swapchainExtent;
}

void Swapchain::chooseSwapchainSurfaceFormat() {
    // Stub implementation - not used in simplified renderer
}

void Swapchain::chooseSwapchainPresentMode() {
    // Stub implementation - not used in simplified renderer
}

void Swapchain::chooseSwapchainExtent() {
    // Stub implementation - not used in simplified renderer
}