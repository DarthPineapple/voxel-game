#include "renderer.h"
#include "engine/window.h"
#include "engine/camera.h"
#include "vulkan/vulkan_instance.h"
#include "vulkan/device.h"
#include "vulkan/swapchain.h"
#include "vulkan/image_views.h"
#include "vulkan/render_pass.h"
#include "vulkan/framebuffers.h"
#include "vulkan/command_pool.h"
#include "vulkan/sync_objects.h"
#include "vulkan/pipeline.h"
#include "vulkan/overlay_pipeline.h"
#include "mesh.h"
#include "world/chunk.h"
#include "world/chunk_manager.h"
#include "world/mesh_generator.h"
#include <iostream>
#include <stdexcept>
#include <cstring>
#include <algorithm>
#include <unordered_map>
#include <tuple>
#include <GLFW/glfw3.h>

Renderer::Renderer()
    : window(nullptr), vulkanInstance(nullptr), device(nullptr), swapchain(nullptr),
      imageViews(nullptr), renderPass(nullptr), framebuffers(nullptr),
      commandPool(nullptr), syncObjects(nullptr), pipeline(nullptr), overlayPipeline(nullptr),
      testMesh(nullptr), overlayVertexBuffer(VK_NULL_HANDLE), overlayVertexBufferMemory(VK_NULL_HANDLE),
      camera(nullptr), uniformBuffers(nullptr), uniformBuffersMemory(nullptr),
      uniformBuffersMapped(nullptr), descriptorPool(VK_NULL_HANDLE),
      descriptorSets(nullptr), currentFrame(0), startTime(0.0) {
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
    
    // Create graphics pipeline with vertex input configuration
    pipeline = new Pipeline(device->getDevice(), renderPass->getRenderPass(), swapchain->getSwapchainExtent());
    pipeline->createPipeline("assets/shaders/shader.vert.spv", "assets/shaders/shader.frag.spv");
    
    // Create overlay pipeline
    overlayPipeline = new OverlayPipeline(device->getDevice(), renderPass->getRenderPass(), swapchain->getSwapchainExtent());
    overlayPipeline->createPipeline("assets/shaders/overlay.vert.spv", "assets/shaders/overlay.frag.spv");
    
    // Create uniform buffers for MVP matrices
    createUniformBuffers();
    
    // Create descriptor pool and sets
    createDescriptorPool();
    createDescriptorSets();
    
    // Create overlay vertex buffer
    createOverlayVertexBuffer();
    
    // Generate test mesh from a chunk
    Chunk testChunk(0, 0, 0);
    testChunk.load();
    
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    MeshGenerator::generateChunkMesh(testChunk, vertices, indices);
    
    // Create mesh and upload to GPU
    testMesh = new Mesh(device->getDevice(), device->getPhysicalDevice());
    testMesh->createVertexBuffer(vertices);
    testMesh->createIndexBuffer(indices);
    
    // Create camera
    camera = new Camera();
    camera->setPosition(8.0f, 8.0f, 20.0f);
    
    // Record start time for animation
    startTime = glfwGetTime();
    
    std::cout << "Vulkan renderer initialized successfully!" << std::endl;
    std::cout << "Generated mesh with " << vertices.size() << " vertices and " 
              << indices.size() << " indices" << std::endl;
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
    
    // Update uniform buffer with current MVP matrix
    updateUniformBuffer(currentFrame);
    
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
    
    // Explicitly reset the command buffer before recording
    vkResetCommandBuffer(commandBuffers[currentFrame], 0);
    
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
    
    // Bind the graphics pipeline
    vkCmdBindPipeline(commandBuffers[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->getPipeline());
    
    // Bind descriptor sets (for uniform buffers)
    vkCmdBindDescriptorSets(commandBuffers[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS,
                           pipeline->getPipelineLayout(), 0, 1, &descriptorSets[currentFrame],
                           0, nullptr);
    
    // Render all chunk meshes
    if (!chunkMeshes.empty()) {
        for (const auto& pair : chunkMeshes) {
            Mesh* mesh = pair.second;
            if (mesh && mesh->getIndexCount() > 0) {
                // Bind vertex buffer
                VkBuffer vertexBuffers[] = {mesh->getVertexBuffer()};
                VkDeviceSize offsets[] = {0};
                vkCmdBindVertexBuffers(commandBuffers[currentFrame], 0, 1, vertexBuffers, offsets);
                
                // Bind index buffer
                vkCmdBindIndexBuffer(commandBuffers[currentFrame], mesh->getIndexBuffer(), 0, VK_INDEX_TYPE_UINT32);
                
                // Draw the mesh
                vkCmdDrawIndexed(commandBuffers[currentFrame], mesh->getIndexCount(), 1, 0, 0, 0);
            }
        }
    } else if (testMesh) {
        // Fallback to test mesh for backward compatibility
        VkBuffer vertexBuffers[] = {testMesh->getVertexBuffer()};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(commandBuffers[currentFrame], 0, 1, vertexBuffers, offsets);
        
        // Bind index buffer
        vkCmdBindIndexBuffer(commandBuffers[currentFrame], testMesh->getIndexBuffer(), 0, VK_INDEX_TYPE_UINT32);
        
        // Draw the mesh
        vkCmdDrawIndexed(commandBuffers[currentFrame], testMesh->getIndexCount(), 1, 0, 0, 0);
    }
    
    // Overlay disabled for now
    // TODO: Re-enable overlay rendering when needed
    // // Draw overlay square
    // // Bind overlay pipeline
    // vkCmdBindPipeline(commandBuffers[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, overlayPipeline->getPipeline());
    // 
    // // Calculate rotation angle based on elapsed time (rotates at 1 radian per second)
    // float currentTime = static_cast<float>(glfwGetTime() - startTime);
    // float angle = currentTime;
    // 
    // // Push the rotation angle constant
    // vkCmdPushConstants(commandBuffers[currentFrame], overlayPipeline->getPipelineLayout(),
    //                   VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(float), &angle);
    // 
    // // Bind overlay vertex buffer
    // VkBuffer overlayBuffers[] = {overlayVertexBuffer};
    // VkDeviceSize overlayOffsets[] = {0};
    // vkCmdBindVertexBuffers(commandBuffers[currentFrame], 0, 1, overlayBuffers, overlayOffsets);
    // 
    // // Draw the overlay square (2 triangles = 6 vertices)
    // vkCmdDraw(commandBuffers[currentFrame], 6, 1, 0, 0);
    
    vkCmdEndRenderPass(commandBuffers[currentFrame]);
    
    if (vkEndCommandBuffer(commandBuffers[currentFrame]) != VK_SUCCESS) {
        throw std::runtime_error("Failed to record command buffer!");
    }
}

void Renderer::cleanup() {
    if (device && device->getDevice() != VK_NULL_HANDLE) {
        vkDeviceWaitIdle(device->getDevice());
    }
    
    // Clean up uniform buffers
    if (uniformBuffers) {
        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            if (uniformBuffers[i] != VK_NULL_HANDLE) {
                vkDestroyBuffer(device->getDevice(), uniformBuffers[i], nullptr);
            }
            if (uniformBuffersMemory && uniformBuffersMemory[i] != VK_NULL_HANDLE) {
                vkFreeMemory(device->getDevice(), uniformBuffersMemory[i], nullptr);
            }
        }
        delete[] uniformBuffers;
        delete[] uniformBuffersMemory;
        uniformBuffers = nullptr;
        uniformBuffersMemory = nullptr;
    }
    
    // Clean up descriptor pool
    if (descriptorPool != VK_NULL_HANDLE) {
        vkDestroyDescriptorPool(device->getDevice(), descriptorPool, nullptr);
        descriptorPool = VK_NULL_HANDLE;
    }
    
    if (descriptorSets) {
        delete[] descriptorSets;
        descriptorSets = nullptr;
    }
    
    if (camera) {
        delete camera;
        camera = nullptr;
    }
    
    // Clean up overlay vertex buffer
    if (overlayVertexBuffer != VK_NULL_HANDLE) {
        vkDestroyBuffer(device->getDevice(), overlayVertexBuffer, nullptr);
        overlayVertexBuffer = VK_NULL_HANDLE;
    }
    if (overlayVertexBufferMemory != VK_NULL_HANDLE) {
        vkFreeMemory(device->getDevice(), overlayVertexBufferMemory, nullptr);
        overlayVertexBufferMemory = VK_NULL_HANDLE;
    }
    
    if (testMesh) {
        testMesh->cleanup();
        delete testMesh;
        testMesh = nullptr;
    }
    
    // Clean up all chunk meshes
    for (auto& pair : chunkMeshes) {
        if (pair.second) {
            pair.second->cleanup();
            delete pair.second;
        }
    }
    chunkMeshes.clear();
    
    if (overlayPipeline) {
        overlayPipeline->cleanup();
        delete overlayPipeline;
        overlayPipeline = nullptr;
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

void Renderer::createUniformBuffers() {
    VkDeviceSize bufferSize = sizeof(float) * 16; // 4x4 matrix
    
    uniformBuffers = new VkBuffer[MAX_FRAMES_IN_FLIGHT];
    uniformBuffersMemory = new VkDeviceMemory[MAX_FRAMES_IN_FLIGHT];
    
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = bufferSize;
        bufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        
        if (vkCreateBuffer(device->getDevice(), &bufferInfo, nullptr, &uniformBuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create uniform buffer!");
        }
        
        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(device->getDevice(), uniformBuffers[i], &memRequirements);
        
        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits,
                                                   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | 
                                                   VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        
        if (vkAllocateMemory(device->getDevice(), &allocInfo, nullptr, &uniformBuffersMemory[i]) != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate uniform buffer memory!");
        }
        
        vkBindBufferMemory(device->getDevice(), uniformBuffers[i], uniformBuffersMemory[i], 0);
    }
}

void Renderer::createDescriptorPool() {
    VkDescriptorPoolSize poolSize{};
    poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSize.descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
    
    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = &poolSize;
    poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
    
    if (vkCreateDescriptorPool(device->getDevice(), &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create descriptor pool!");
    }
}

void Renderer::createDescriptorSets() {
    VkDescriptorSetLayout layouts[MAX_FRAMES_IN_FLIGHT];
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        layouts[i] = pipeline->getDescriptorSetLayout();
    }
    
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
    allocInfo.pSetLayouts = layouts;
    
    descriptorSets = new VkDescriptorSet[MAX_FRAMES_IN_FLIGHT];
    if (vkAllocateDescriptorSets(device->getDevice(), &allocInfo, descriptorSets) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate descriptor sets!");
    }
    
    // Update descriptor sets
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = uniformBuffers[i];
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(float) * 16;
        
        VkWriteDescriptorSet descriptorWrite{};
        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet = descriptorSets[i];
        descriptorWrite.dstBinding = 0;
        descriptorWrite.dstArrayElement = 0;
        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.pBufferInfo = &bufferInfo;
        
        vkUpdateDescriptorSets(device->getDevice(), 1, &descriptorWrite, 0, nullptr);
    }
}

void Renderer::updateUniformBuffer(uint32_t currentImage) {
    float aspectRatio = swapchain->getSwapchainExtent().width / 
                       (float)swapchain->getSwapchainExtent().height;
    
    float mvp[16];
    camera->getMVPMatrix(mvp, aspectRatio);
    
    void* data;
    vkMapMemory(device->getDevice(), uniformBuffersMemory[currentImage], 0, sizeof(mvp), 0, &data);
    std::memcpy(data, mvp, sizeof(mvp));
    vkUnmapMemory(device->getDevice(), uniformBuffersMemory[currentImage]);
}

uint32_t Renderer::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(device->getPhysicalDevice(), &memProperties);
    
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && 
            (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }
    
    throw std::runtime_error("Failed to find suitable memory type!");
}

void Renderer::logMeshInfo() const {
    if (!testMesh) {
        std::cout << "[Mesh] No mesh available" << std::endl;
        return;
    }
    
    std::cout << "[Mesh] Vertex count: " << testMesh->getVertexCount() << std::endl;
    std::cout << "[Mesh] Index count: " << testMesh->getIndexCount() << std::endl;
    std::cout << "[Mesh] Triangle count: " << (testMesh->getIndexCount() / 3) << std::endl;
    std::cout << "[Mesh] Vertex buffer: " << testMesh->getVertexBuffer() << std::endl;
    std::cout << "[Mesh] Index buffer: " << testMesh->getIndexBuffer() << std::endl;
    std::cout << "[Mesh] Vertex buffer size: " << (testMesh->getVertexCount() * sizeof(Vertex)) 
              << " bytes" << std::endl;
    std::cout << "[Mesh] Index buffer size: " << (testMesh->getIndexCount() * sizeof(uint32_t)) 
              << " bytes" << std::endl;
    
    // Log sample vertices (first 3 and last 3)
    const auto& vertices = testMesh->getVertices();
    if (!vertices.empty()) {
        size_t sampleCount = std::min(size_t(3), vertices.size());
        std::cout << "[Mesh] Sample vertices (first " << sampleCount << "):" << std::endl;
        for (size_t i = 0; i < sampleCount; i++) {
            const Vertex& v = vertices[i];
            std::cout << "  Vertex " << i << ": pos(" << v.position[0] << ", " 
                      << v.position[1] << ", " << v.position[2] << ") "
                      << "normal(" << v.normal[0] << ", " << v.normal[1] << ", " 
                      << v.normal[2] << ") "
                      << "uv(" << v.texCoord[0] << ", " << v.texCoord[1] << ")" << std::endl;
        }
        
        if (vertices.size() > 3) {
            std::cout << "[Mesh] Sample vertices (last " << sampleCount << "):" << std::endl;
            for (size_t i = vertices.size() - sampleCount; i < vertices.size(); i++) {
                const Vertex& v = vertices[i];
                std::cout << "  Vertex " << i << ": pos(" << v.position[0] << ", " 
                          << v.position[1] << ", " << v.position[2] << ") "
                          << "normal(" << v.normal[0] << ", " << v.normal[1] << ", " 
                          << v.normal[2] << ") "
                          << "uv(" << v.texCoord[0] << ", " << v.texCoord[1] << ")" << std::endl;
            }
        }
    }
}

void Renderer::logTransformedMeshInfo() const {
    if (!testMesh || !camera) {
        std::cout << "[Transformed Mesh] No mesh or camera available" << std::endl;
        return;
    }
    
    // Get MVP matrix
    float aspectRatio = swapchain->getSwapchainExtent().width / 
                       (float)swapchain->getSwapchainExtent().height;
    float mvp[16];
    camera->getMVPMatrix(mvp, aspectRatio);
    
    std::cout << "[Transform] MVP Matrix:" << std::endl;
    for (int row = 0; row < 4; row++) {
        std::cout << "  [";
        for (int col = 0; col < 4; col++) {
            std::cout << mvp[col * 4 + row];
            if (col < 3) std::cout << ", ";
        }
        std::cout << "]" << std::endl;
    }
    
    std::cout << "[Transform] Viewport: " << swapchain->getSwapchainExtent().width 
              << "x" << swapchain->getSwapchainExtent().height << std::endl;
    std::cout << "[Transform] Aspect ratio: " << aspectRatio << std::endl;
    
    // Transform sample vertices
    const auto& vertices = testMesh->getVertices();
    if (!vertices.empty()) {
        size_t sampleCount = std::min(size_t(3), vertices.size());
        std::cout << "[Transform] Sample transformed vertices (first " << sampleCount << "):" << std::endl;
        
        for (size_t i = 0; i < sampleCount; i++) {
            const Vertex& v = vertices[i];
            
            // Apply MVP transformation
            float x = v.position[0];
            float y = v.position[1];
            float z = v.position[2];
            float w = 1.0f;
            
            float tx = mvp[0] * x + mvp[4] * y + mvp[8] * z + mvp[12] * w;
            float ty = mvp[1] * x + mvp[5] * y + mvp[9] * z + mvp[13] * w;
            float tz = mvp[2] * x + mvp[6] * y + mvp[10] * z + mvp[14] * w;
            float tw = mvp[3] * x + mvp[7] * y + mvp[11] * z + mvp[15] * w;
            
            std::cout << "  Vertex " << i << ": " << std::endl;
            std::cout << "    Original: (" << x << ", " << y << ", " << z << ")" << std::endl;
            std::cout << "    Clip space: (" << tx << ", " << ty << ", " << tz << ", " << tw << ")" << std::endl;
            
            // Perspective divide for NDC
            if (tw != 0.0f) {
                float ndcX = tx / tw;
                float ndcY = ty / tw;
                float ndcZ = tz / tw;
                std::cout << "    NDC: (" << ndcX << ", " << ndcY << ", " << ndcZ << ")" << std::endl;
            } else {
                std::cout << "    NDC: Division by zero (w=0)" << std::endl;
            }
        }
    }
}

void Renderer::createOverlayVertexBuffer() {
    // Define a square as two triangles (6 vertices in 2D)
    // Square centered at origin with size 2.0 (from -1 to 1)
    float vertices[] = {
        // First triangle
        -1.0f, -1.0f,  // Bottom-left
         1.0f, -1.0f,  // Bottom-right
         1.0f,  1.0f,  // Top-right
        
        // Second triangle
         1.0f,  1.0f,  // Top-right
        -1.0f,  1.0f,  // Top-left
        -1.0f, -1.0f   // Bottom-left
    };
    
    VkDeviceSize bufferSize = sizeof(vertices);
    
    // Create buffer
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = bufferSize;
    bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    
    if (vkCreateBuffer(device->getDevice(), &bufferInfo, nullptr, &overlayVertexBuffer) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create overlay vertex buffer!");
    }
    
    // Get memory requirements
    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device->getDevice(), overlayVertexBuffer, &memRequirements);
    
    // Allocate memory
    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits,
                                               VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | 
                                               VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    
    if (vkAllocateMemory(device->getDevice(), &allocInfo, nullptr, &overlayVertexBufferMemory) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate overlay vertex buffer memory!");
    }
    
    // Bind memory to buffer
    vkBindBufferMemory(device->getDevice(), overlayVertexBuffer, overlayVertexBufferMemory, 0);
    
    // Copy vertex data to buffer
    void* data;
    vkMapMemory(device->getDevice(), overlayVertexBufferMemory, 0, bufferSize, 0, &data);
    std::memcpy(data, vertices, (size_t)bufferSize);
    vkUnmapMemory(device->getDevice(), overlayVertexBufferMemory);
}

Mesh* Renderer::createMeshForChunk(Chunk* chunk) {
    if (!chunk) return nullptr;
    
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    MeshGenerator::generateChunkMesh(*chunk, vertices, indices);
    
    // Only create mesh if there are vertices
    if (vertices.empty() || indices.empty()) {
        return nullptr;
    }
    
    Mesh* mesh = new Mesh(device->getDevice(), device->getPhysicalDevice());
    mesh->createVertexBuffer(vertices);
    mesh->createIndexBuffer(indices);
    
    return mesh;
}

void Renderer::updateChunkMeshes(ChunkManager* chunkManager) {
    if (!chunkManager) return;
    
    const auto& chunks = chunkManager->getChunks();
    
    // Track which chunks should have meshes
    std::unordered_map<std::tuple<int, int, int>, bool, TupleHash> activeChunks;
    
    // Create meshes for new chunks
    for (Chunk* chunk : chunks) {
        auto key = std::make_tuple(chunk->getPosX(), chunk->getPosY(), chunk->getPosZ());
        activeChunks[key] = true;
        
        // Create mesh if it doesn't exist
        if (chunkMeshes.find(key) == chunkMeshes.end()) {
            Mesh* mesh = createMeshForChunk(chunk);
            if (mesh) {
                chunkMeshes[key] = mesh;
            }
        }
    }
    
    // Remove meshes for chunks that no longer exist
    std::vector<std::tuple<int, int, int>> meshesToRemove;
    for (const auto& pair : chunkMeshes) {
        if (activeChunks.find(pair.first) == activeChunks.end()) {
            meshesToRemove.push_back(pair.first);
        }
    }
    
    for (const auto& key : meshesToRemove) {
        auto it = chunkMeshes.find(key);
        if (it != chunkMeshes.end()) {
            if (it->second) {
                it->second->cleanup();
                delete it->second;
            }
            chunkMeshes.erase(it);
        }
    }
}