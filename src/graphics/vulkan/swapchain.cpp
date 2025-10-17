#include "swapchain.h"
#include "engine/window.h"
#include <stdexcept>
#include <vector>
#include <algorithm>

Swapchain::Swapchain(VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface)
    : device(device), physicalDevice(physicalDevice), surface(surface), 
      swapchain(VK_NULL_HANDLE), swapchainImageFormat(VK_FORMAT_UNDEFINED), 
      swapchainExtent{0, 0} {
}

Swapchain::~Swapchain() {
    cleanup();
}

void Swapchain::createSwapchain(Window* window) {
    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &capabilities);

    uint32_t formatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);
    if (formatCount == 0) {
        throw std::runtime_error("No surface formats available.");
    }
    std::vector<VkSurfaceFormatKHR> formats(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, formats.data());

    uint32_t presentModeCount = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);
    if (presentModeCount == 0) {
        throw std::runtime_error("No present modes available.");
    }
    std::vector<VkPresentModeKHR> presentModes(presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, presentModes.data());

    VkSurfaceFormatKHR surfaceFormat = chooseSwapchainSurfaceFormat(formats);
    VkPresentModeKHR presentMode = chooseSwapchainPresentMode(presentModes);
    VkExtent2D extent = chooseSwapchainExtent(capabilities, window);

    uint32_t imageCount = capabilities.minImageCount + 1;
    if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount) {
        imageCount = capabilities.maxImageCount;
    }

    // Determine sharing mode based on queue families
    int graphicsFamily = -1, presentFamily = -1;
    {
        uint32_t qCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &qCount, nullptr);
        std::vector<VkQueueFamilyProperties> qProps(qCount);
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &qCount, qProps.data());
        for (size_t i = 0; i < qProps.size(); ++i) {
            if (qProps[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) graphicsFamily = static_cast<int>(i);
            VkBool32 presentSupport = VK_FALSE;
            vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, static_cast<uint32_t>(i), surface, &presentSupport);
            if (presentSupport) presentFamily = static_cast<int>(i);
            if (graphicsFamily >= 0 && presentFamily >= 0) break;
        }
    }

    uint32_t queueFamilyIndices[2] = { static_cast<uint32_t>(graphicsFamily), static_cast<uint32_t>(presentFamily) };

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    if (graphicsFamily != presentFamily && graphicsFamily >= 0 && presentFamily >= 0) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    createInfo.preTransform = capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapchain) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create swap chain!");
    }

    vkGetSwapchainImagesKHR(device, swapchain, &imageCount, nullptr);
    swapchainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(device, swapchain, &imageCount, swapchainImages.data());

    swapchainImageFormat = surfaceFormat.format;
    swapchainExtent = extent;
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

VkSurfaceFormatKHR Swapchain::chooseSwapchainSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats) const {
    if (formats.size() == 1 && formats[0].format == VK_FORMAT_UNDEFINED) {
        return {VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};
    }

    for (const auto& f : formats) {
        if (f.format == VK_FORMAT_B8G8R8A8_UNORM && f.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return f;
        }
    }
    return formats[0];
}

VkPresentModeKHR Swapchain::chooseSwapchainPresentMode(const std::vector<VkPresentModeKHR>& presentModes) const {
    for (const auto& pm : presentModes) {
        if (pm == VK_PRESENT_MODE_MAILBOX_KHR) {
            return pm;
        }
    }
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D Swapchain::chooseSwapchainExtent(const VkSurfaceCapabilitiesKHR& capabilities, Window* window) const {
    if (capabilities.currentExtent.width != UINT32_MAX) {
        return capabilities.currentExtent;
    } else {
        VkExtent2D actualExtent = {
            static_cast<uint32_t>(window->getWidth()),
            static_cast<uint32_t>(window->getHeight())
        };
        actualExtent.width = std::max(capabilities.minImageExtent.width,
                                     std::min(capabilities.maxImageExtent.width, actualExtent.width));
        actualExtent.height = std::max(capabilities.minImageExtent.height,
                                      std::min(capabilities.maxImageExtent.height, actualExtent.height));
        return actualExtent;
    }
}