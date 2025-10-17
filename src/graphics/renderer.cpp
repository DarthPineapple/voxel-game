#include "renderer.h"
#include "engine/window.h"
#include "vulkan/vulkan_instance.h"
#include "vulkan/device.h"
#include "vulkan/swapchain.h"
#include "vulkan/image_views.h"
#include "vulkan/render_pass.h"
#include "vulkan/framebuffers.h"
#include "vulkan/command_pool.h"
#include "vulkan/sync_objects.h"
#include "vulkan/pipeline.h"
#include <iostream>
#include <stdexcept>

Renderer::Renderer()
    : window(nullptr), vulkanInstance(nullptr), device(nullptr), swapchain(nullptr),
      imageViews(nullptr), renderPass(nullptr), framebuffers(nullptr),
      commandPool(nullptr), syncObjects(nullptr), pipeline(nullptr),
      currentFrame(0) {
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
    
    // Create image views
    imageViews = new ImageViews(device->getDevice());
    imageViews->createImageViews(swapchain->getSwapchainImages(), swapchain->getSwapchainImageFormat());
    
    // Create render pass
    renderPass = new RenderPass(device->getDevice());
    renderPass->createRenderPass(swapchain->getSwapchainImageFormat());
    
    // Create framebuffers
    framebuffers = new Framebuffers(device->getDevice());
    framebuffers->createFramebuffers(imageViews->getImageViews(), 
                                    renderPass->getRenderPass(),
                                    swapchain->getSwapchainExtent());
    
    // Create command pool and buffers
    commandPool = new CommandPool(device->getDevice(), device->getPhysicalDevice(), vulkanInstance->getSurface());
    commandPool->createCommandPool();
    commandPool->createCommandBuffers(MAX_FRAMES_IN_FLIGHT);
    
    // Create synchronization objects
    syncObjects = new SyncObjects(device->getDevice());
    syncObjects->createSyncObjects(MAX_FRAMES_IN_FLIGHT);
    
    // Create graphics pipeline (Note: this would fail without compiled shaders)
    // Commenting out for now to allow the program to run
    // pipeline = new Pipeline(device->getDevice(), renderPass->getRenderPass(), swapchain->getSwapchainExtent());
    // pipeline->createPipeline("assets/shaders/shader.vert.spv", "assets/shaders/shader.frag.spv");
    
    std::cout << "Vulkan renderer initialized successfully!" << std::endl;
}

void Renderer::render() {
    // Wait for the previous frame to finish
    const auto& fences = syncObjects->getInFlightFences();
    vkWaitForFences(device->getDevice(), 1, &fences[currentFrame], VK_TRUE, UINT64_MAX);
    
    // Acquire an image from the swapchain
    uint32_t imageIndex;
    const auto& imageAvailable = syncObjects->getImageAvailableSemaphores();
    VkResult result = vkAcquireNextImageKHR(device->getDevice(), 
                                           swapchain->getSwapchain(), 
                                           UINT64_MAX,
                                           imageAvailable[currentFrame], 
                                           VK_NULL_HANDLE, 
                                           &imageIndex);
    
    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        // Swapchain needs to be recreated (window resized, etc.)
        return;
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("Failed to acquire swapchain image!");
    }
    
    // Reset the fence for this frame
    vkResetFences(device->getDevice(), 1, &fences[currentFrame]);
    
    // Record command buffer
    recordCommandBuffer(imageIndex);
    
    // Submit command buffer
    const auto& renderFinished = syncObjects->getRenderFinishedSemaphores();
    const auto& commandBuffers = commandPool->getCommandBuffers();
    
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    
    VkSemaphore waitSemaphores[] = {imageAvailable[currentFrame]};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffers[currentFrame];
    
    VkSemaphore signalSemaphores[] = {renderFinished[currentFrame]};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;
    
    if (vkQueueSubmit(device->getGraphicsQueue(), 1, &submitInfo, fences[currentFrame]) != VK_SUCCESS) {
        throw std::runtime_error("Failed to submit draw command buffer!");
    }
    
    // Present the image
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;
    
    VkSwapchainKHR swapchains[] = {swapchain->getSwapchain()};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapchains;
    presentInfo.pImageIndices = &imageIndex;
    
    result = vkQueuePresentKHR(device->getPresentQueue(), &presentInfo);
    
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        // Swapchain needs to be recreated
    } else if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to present swapchain image!");
    }
    
    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void Renderer::recordCommandBuffer(size_t imageIndex) {
    const auto& commandBuffers = commandPool->getCommandBuffers();
    
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    
    if (vkBeginCommandBuffer(commandBuffers[currentFrame], &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("Failed to begin recording command buffer!");
    }
    
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderPass->getRenderPass();
    renderPassInfo.framebuffer = framebuffers->getFramebuffers()[imageIndex];
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = swapchain->getSwapchainExtent();
    
    VkClearValue clearColor = {{{0.0f, 0.0f, 0.2f, 1.0f}}};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;
    
    vkCmdBeginRenderPass(commandBuffers[currentFrame], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    
    // Draw commands would go here
    // For now, just clear the screen
    
    vkCmdEndRenderPass(commandBuffers[currentFrame]);
    
    if (vkEndCommandBuffer(commandBuffers[currentFrame]) != VK_SUCCESS) {
        throw std::runtime_error("Failed to record command buffer!");
    }
}

void Renderer::cleanup() {
    if (device && device->getDevice() != VK_NULL_HANDLE) {
        vkDeviceWaitIdle(device->getDevice());
    }
    
    if (pipeline) {
        pipeline->cleanup();
        delete pipeline;
        pipeline = nullptr;
    }
    
    if (syncObjects) {
        syncObjects->cleanup();
        delete syncObjects;
        syncObjects = nullptr;
    }
    
    if (commandPool) {
        commandPool->cleanup();
        delete commandPool;
        commandPool = nullptr;
    }
    
    if (framebuffers) {
        framebuffers->cleanup();
        delete framebuffers;
        framebuffers = nullptr;
    }
    
    if (renderPass) {
        renderPass->cleanup();
        delete renderPass;
        renderPass = nullptr;
    }
    
    if (imageViews) {
        imageViews->cleanup();
        delete imageViews;
        imageViews = nullptr;
    }
    
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