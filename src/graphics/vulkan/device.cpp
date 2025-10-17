#include "device.h"
#include <stdexcept>
#include <iostream>
#include <vector>
#include <set>
#include <string>

// Fallback for older Vulkan headers that don't define this macro
#ifndef VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME
#define VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME "VK_KHR_portability_subset"
#endif

Device::Device() : device(VK_NULL_HANDLE), physicalDevice(VK_NULL_HANDLE), 
                   graphicsQueue(VK_NULL_HANDLE), presentQueue(VK_NULL_HANDLE) {
}

Device::~Device() {
    cleanup();
}

void Device::pickPhysicalDevice(VkInstance instance, VkSurfaceKHR surface) {
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    if (deviceCount == 0) {
        throw std::runtime_error("Failed to find GPUs with Vulkan support!");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    physicalDevice = VK_NULL_HANDLE;
    for (auto dev : devices) {
        if (!deviceSupportsExtensions(dev)) continue;
        int gfx = -1, present = -1;
        if (!findQueueFamilies(dev, surface, gfx, present)) continue;
        if (!swapchainAdequate(dev, surface)) continue;
        physicalDevice = dev;
        break;
    }

    if (physicalDevice == VK_NULL_HANDLE) {
        throw std::runtime_error("Failed to find a suitable GPU!");
    }
}

void Device::createLogicalDevice(VkSurfaceKHR surface) {
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

    int graphicsFamily = -1;
    int presentFamily = -1;

    for (uint32_t i = 0; i < queueFamilies.size(); i++) {
        if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            graphicsFamily = i;
        }
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport);
        if (presentSupport) {
            presentFamily = i;
        }
        if (graphicsFamily >= 0 && presentFamily >= 0) {
            break;
        }
    }

    if (graphicsFamily < 0 || presentFamily < 0) {
        throw std::runtime_error("Failed to find suitable queue families!");
    }

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {static_cast<uint32_t>(graphicsFamily), static_cast<uint32_t>(presentFamily)};

    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures{};

    // Required device extensions
    std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
#ifdef __APPLE__
    deviceExtensions.push_back(VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME);
#endif

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();
    createInfo.enabledLayerCount = 0;

    if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create logical device!");
    }

    vkGetDeviceQueue(device, graphicsFamily, 0, &graphicsQueue);
    vkGetDeviceQueue(device, presentFamily, 0, &presentQueue);
}

bool Device::deviceSupportsExtensions(VkPhysicalDevice dev) const {
    uint32_t extCount = 0;
    vkEnumerateDeviceExtensionProperties(dev, nullptr, &extCount, nullptr);
    std::vector<VkExtensionProperties> available(extCount);
    vkEnumerateDeviceExtensionProperties(dev, nullptr, &extCount, available.data());

    std::set<std::string> required = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
#ifdef __APPLE__
    required.insert(VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME);
#endif
    for (const auto& e : available) {
        required.erase(e.extensionName);
    }
    return required.empty();
}

bool Device::findQueueFamilies(VkPhysicalDevice dev, VkSurfaceKHR surface, int& gfx, int& present) const {
    gfx = -1; 
    present = -1;
    uint32_t count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(dev, &count, nullptr);
    std::vector<VkQueueFamilyProperties> props(count);
    vkGetPhysicalDeviceQueueFamilyProperties(dev, &count, props.data());

    for (uint32_t i = 0; i < count; ++i) {
        if (props[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            gfx = static_cast<int>(i);
        }
        VkBool32 presentSupport = VK_FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(dev, i, surface, &presentSupport);
        if (presentSupport) {
            present = static_cast<int>(i);
        }
        if (gfx >= 0 && present >= 0) break;
    }
    return (gfx >= 0 && present >= 0);
}

bool Device::swapchainAdequate(VkPhysicalDevice dev, VkSurfaceKHR surface) const {
    uint32_t formatCount = 0, presentModeCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(dev, surface, &formatCount, nullptr);
    vkGetPhysicalDeviceSurfacePresentModesKHR(dev, surface, &presentModeCount, nullptr);
    return formatCount > 0 && presentModeCount > 0;
}

void Device::cleanup() {
    if (device != VK_NULL_HANDLE) {
        vkDeviceWaitIdle(device);
        vkDestroyDevice(device, nullptr);
        device = VK_NULL_HANDLE;
    }
}