#include "renderer.h"
#include "vulkan/device.h"
#include "vulkan/pipeline.h"
#include "vulkan/swapchain.h"
#include "../world/chunk_manager.h"

Renderer::Renderer(Device& device, Swapchain& swapchain, ChunkManager& chunkManager)
    : device(device), swapchain(swapchain), chunkManager(chunkManager) {
}

void Renderer::init() {
    pipeline.createPipeline(device);
}

void Renderer::render() {
    // Begin the frame
    swapchain.beginFrame();

    // Render each chunk
    for (const auto& chunk : chunkManager.getChunks()) {
        // Render the chunk
        pipeline.bind(chunk);
        chunk.draw();
    }

    // End the frame
    swapchain.endFrame();
}

void Renderer::cleanup() {
    pipeline.cleanup();
}