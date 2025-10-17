#include "renderer.h"
#include "engine/window.h"
#include "vulkan/vulkan_instance.h"
#include "vulkan/device.h"
#include "vulkan/swapchain.h"
#include <iostream>
#include <stdexcept>

Renderer::Renderer()
    : window(nullptr), vulkanInstance(nullptr), device(nullptr), swapchain(nullptr) {
}

Renderer::~Renderer() {
    cleanup();
}

void Renderer::init(Window* window) {
    this->window = window;
    
    // Create Vulkan instance and surface
    vulkanInstance = new VulkanInstance();
    vulkanInstance->createInstance();
    vulkanInstance->createSurface(window);
    
    // Create device
    device = new Device();
    device->pickPhysicalDevice(vulkanInstance->getInstance(), vulkanInstance->getSurface());
    device->createLogicalDevice(vulkanInstance->getSurface());
    
    // Create swapchain
    swapchain = new Swapchain(device->getDevice(), device->getPhysicalDevice(), vulkanInstance->getSurface());
    swapchain->createSwapchain(window);
    
    std::cout << "Vulkan renderer initialized successfully!" << std::endl;
}

void Renderer::render() {
    // Basic render loop - just a placeholder for now
    // In a full implementation, this would acquire swapchain images,
    // record command buffers, and present frames
}

void Renderer::cleanup() {
    if (swapchain) {
        swapchain->cleanup();
        delete swapchain;
        swapchain = nullptr;
    }
    
    if (device) {
        device->cleanup();
        delete device;
        device = nullptr;
    }
    
    if (vulkanInstance) {
        vulkanInstance->cleanup();
        delete vulkanInstance;
        vulkanInstance = nullptr;
    }
}