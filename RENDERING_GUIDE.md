# Rendering System Guide

This guide explains the rendering architecture and how to use the mesh generation system.

## Architecture Overview

The rendering system follows a modular architecture with clear separation of concerns:

```
Renderer (orchestration)
├── VulkanInstance (instance, surface)
├── Device (physical device, logical device, queues)
├── Swapchain (swapchain, images)
├── ImageViews (image views for swapchain images)
├── RenderPass (render pass configuration)
├── Framebuffers (framebuffers for each image view)
├── CommandPool (command pool and command buffers)
├── SyncObjects (semaphores and fences)
└── Pipeline (graphics pipeline, shader loading)
```

## Rendering Pipeline Flow

1. **Initialization** (`Renderer::init`)
   - Creates all Vulkan components in order
   - Sets up synchronization objects
   - Loads and compiles shaders into pipeline

2. **Frame Rendering** (`Renderer::render`)
   - Wait for previous frame to complete (fence)
   - Acquire next swapchain image (semaphore)
   - Record command buffer with render pass
   - Submit command buffer to graphics queue
   - Present image to display

3. **Cleanup** (`Renderer::cleanup`)
   - Waits for device idle
   - Destroys all Vulkan objects in reverse order

## Mesh Generation System

### Basic Usage

```cpp
#include "world/chunk.h"
#include "world/mesh_generator.h"
#include "graphics/mesh.h"

// Create and load a chunk
Chunk chunk(0, 0, 0);
chunk.load();

// Generate mesh from chunk voxels
std::vector<Vertex> vertices;
std::vector<uint32_t> indices;
MeshGenerator::generateChunkMesh(chunk, vertices, indices);

// Create GPU buffers
Mesh* mesh = new Mesh(device->getDevice(), device->getPhysicalDevice());
mesh->createVertexBuffer(vertices);
mesh->createIndexBuffer(indices);
```

### Vertex Structure

The `Vertex` struct defines the layout for mesh data (defined in `src/graphics/mesh.h`):

```cpp
struct Vertex {
    float position[3];  // World space position (x, y, z)
    float normal[3];    // Surface normal (nx, ny, nz)
    float texCoord[2];  // Texture coordinates (u, v)
};
```

Note: The actual implementation uses C-style arrays for direct memory layout control, which is common practice for GPU vertex data.

This layout matches the shader expectations with:
- Location 0: vec3 position
- Location 1: vec3 normal
- Location 2: vec2 texCoord

### Mesh Generation Algorithm

The mesh generator uses a greedy meshing algorithm:

1. Sweeps through each axis (X, Y, Z) to generate faces perpendicular to that axis
2. For each slice, builds a mask of exposed faces
3. Merges adjacent exposed faces with the same voxel type into larger rectangles
4. Generates one quad per merged rectangular region instead of per voxel face
5. Calculates normals and texture coordinates for each merged quad

This approach provides massive performance improvements:
- **Solid chunks**: 99.6% reduction in quad count (1536 → 6 quads)
- **Terrain-like chunks**: 99.4% reduction (1024 → 6 quads)
- **Typical use cases**: 95-99% fewer vertices and indices

The algorithm handles all voxel configurations correctly while maintaining optimal performance.

## Shader System

### Shader Compilation

Before building the project, compile shaders to SPIR-V:

```bash
./compile_shaders.sh
```

This compiles:
- `assets/shaders/shader.vert` → `shader.vert.spv`
- `assets/shaders/shader.frag` → `shader.frag.spv`

### Current Test Shader

The current vertex shader renders a hardcoded triangle for testing:

```glsl
#version 450

vec2 positions[3] = vec2[](
    vec2(0.0, -0.5),
    vec2(0.5, 0.5),
    vec2(-0.5, 0.5)
);

void main() {
    gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
}
```

### Next Steps for Shader Integration

To render actual voxel meshes:

1. Update vertex shader to accept vertex input:
   ```glsl
   layout(location = 0) in vec3 inPosition;
   layout(location = 1) in vec3 inNormal;
   layout(location = 2) in vec2 inTexCoord;
   ```

2. Add uniform buffer for MVP matrices:
   ```glsl
   layout(set = 0, binding = 0) uniform MVP {
       mat4 model;
       mat4 view;
       mat4 projection;
   };
   ```

3. Update Pipeline creation to include vertex input state:
   ```cpp
   VkVertexInputBindingDescription bindingDescription{};
   bindingDescription.binding = 0;
   bindingDescription.stride = sizeof(Vertex);
   bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
   
   std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};
   // Position
   attributeDescriptions[0].binding = 0;
   attributeDescriptions[0].location = 0;
   attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
   attributeDescriptions[0].offset = offsetof(Vertex, position);
   // Normal
   attributeDescriptions[1].binding = 0;
   attributeDescriptions[1].location = 1;
   attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
   attributeDescriptions[1].offset = offsetof(Vertex, normal);
   // TexCoord
   attributeDescriptions[2].binding = 0;
   attributeDescriptions[2].location = 2;
   attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
   attributeDescriptions[2].offset = offsetof(Vertex, texCoord);
   ```

## Command Buffer Recording

Command buffers are recorded per frame in `Renderer::recordCommandBuffer`. Currently, the implementation only clears the screen:

```cpp
// Begin command buffer
vkBeginCommandBuffer(commandBuffer, &beginInfo);

// Begin render pass (clears to dark blue)
vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

// TODO: Draw commands will be added here for mesh rendering
// Example for future mesh rendering:
//   vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
//   vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vertexBuffer, &offset);
//   vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);
//   vkCmdDrawIndexed(commandBuffer, indexCount, 1, 0, 0, 0);

// End render pass and command buffer
vkCmdEndRenderPass(commandBuffer);
vkEndCommandBuffer(commandBuffer);
```

## Synchronization

The renderer uses double buffering with 2 frames in flight:

- **Image Available Semaphore**: Signals when swapchain image is ready
- **Render Finished Semaphore**: Signals when rendering is complete
- **In-Flight Fence**: Ensures CPU doesn't get too far ahead of GPU

Each frame waits on the fence from 2 frames ago, preventing resource contention.

## Memory Management

### Buffer Creation

Buffers are created with appropriate usage flags:
- Vertex buffers: `VK_BUFFER_USAGE_VERTEX_BUFFER_BIT`
- Index buffers: `VK_BUFFER_USAGE_INDEX_BUFFER_BIT`

Memory is allocated with:
- `VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT`: CPU can write
- `VK_MEMORY_PROPERTY_HOST_COHERENT_BIT`: Automatic synchronization

### Cleanup Order

Resources are destroyed in reverse order of creation to maintain dependencies:
1. Pipeline and pipeline layout
2. Synchronization objects
3. Command pool (automatically frees buffers)
4. Framebuffers
5. Render pass
6. Image views
7. Swapchain
8. Device
9. Surface and instance

## Performance Considerations

### Mesh Generation

- Greedy meshing reduces quad count by 95-99% in typical scenarios
- Merged quads significantly reduce vertex/index buffer sizes
- GPU memory usage is dramatically reduced
- Pre-allocate vertex/index vectors for known maximum size

### Rendering

- Use staging buffers for large mesh uploads
- Implement frustum culling to skip invisible chunks
- Consider instanced rendering for multiple chunks
- Use descriptor sets for per-chunk data

## Troubleshooting

### Validation Layers

Enable validation layers for debugging:
```cpp
// In VulkanInstance::createInstance()
std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};
createInfo.enabledLayerCount = validationLayers.size();
createInfo.ppEnabledLayerNames = validationLayers.data();
```

### Common Issues

1. **Black screen**: Check if command buffers are being recorded and submitted
2. **Validation errors**: Ensure all Vulkan objects are created in correct order
3. **Crash on cleanup**: Verify `vkDeviceWaitIdle` is called before destroying resources
4. **Memory leaks**: Ensure all `create*` calls have matching `cleanup()` calls

## Future Enhancements

- [ ] Implement uniform buffer objects for MVP matrices
- [ ] Add descriptor sets for textures and uniforms
- [ ] Implement dynamic state for viewport/scissor
- [ ] Add depth buffer for 3D rendering
- [ ] Implement multi-sampling anti-aliasing
- [ ] Add push constants for per-draw data
