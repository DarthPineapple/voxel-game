# Voxel Game Architecture

## Project Structure

The project is organized into logical modules for maintainability and scalability:

```
src/
├── engine/          # Core engine components
│   ├── application  # Main application loop and lifecycle
│   └── window       # Window management (GLFW)
│
├── graphics/        # Rendering system
│   ├── renderer     # High-level renderer orchestration
│   ├── mesh         # Vertex and index buffer management
│   └── vulkan/      # Vulkan-specific components
│       ├── vulkan_instance  # Instance and surface creation
│       ├── device           # Physical/logical device management
│       ├── swapchain        # Swapchain creation and management
│       ├── image_views      # Image views for swapchain images
│       ├── render_pass      # Render pass configuration
│       ├── framebuffers     # Framebuffer creation
│       ├── command_pool     # Command pool and buffers
│       ├── sync_objects     # Synchronization primitives
│       └── pipeline         # Graphics pipeline and shader loading
│
├── world/           # Voxel world management
│   ├── voxel        # Individual voxel representation
│   ├── chunk        # Chunk data structure (16x16x16 voxels)
│   ├── chunk_manager # Chunk loading/unloading system
│   └── mesh_generator # Greedy meshing for voxel chunks
│
└── utils/           # Utility functions
    └── math_utils   # Math helpers (Vec3, lerp, clamp, etc.)
```

## Design Principles

### Modularity
- Each component has a single, well-defined responsibility
- Vulkan initialization is split into logical units:
  - VulkanInstance: Instance and surface creation
  - Device: Physical device selection and logical device creation
  - Swapchain: Swapchain configuration and creation
  - Renderer: High-level orchestration of all components

### Scalability
- No single file exceeds 165 lines; most are under 100 lines (previously: renderer.cpp was 351 lines)
- Clear separation of concerns allows parallel development
- Easy to add new rendering features without modifying core components

### Readability
- Descriptive class and method names
- Logical file organization mirrors system architecture
- Each Vulkan subsystem is self-contained

### Debuggability
- Isolated components can be tested independently
- Clear initialization order in Renderer::init()
- Explicit error handling at each layer

## Component Interactions

```
Application
    └── Renderer
        ├── VulkanInstance (instance, surface)
        ├── Device (physical device, logical device, queues)
        ├── Swapchain (swapchain, images)
        ├── ImageViews (image views)
        ├── RenderPass (render pass)
        ├── Framebuffers (framebuffers)
        ├── CommandPool (command pool, command buffers)
        ├── SyncObjects (semaphores, fences)
        └── Pipeline (graphics pipeline, shaders)
```

## Recent Enhancements

The following components have been implemented following the modular pattern:

1. ✅ **Image Views**: Create views for swapchain images
2. ✅ **Render Pass**: Configure rendering pipeline stages
3. ✅ **Framebuffers**: Link image views to render pass
4. ✅ **Command Pool/Buffers**: Allocate and record GPU commands
5. ✅ **Synchronization**: Semaphores and fences for frame coordination
6. ✅ **Graphics Pipeline**: Shader loading and pipeline state
7. ✅ **Mesh System**: Vertex/index buffer management
8. ✅ **Mesh Generator**: Greedy meshing for voxel chunks

## Future Enhancements

Based on the roadmap in README.md:
1. Integrate mesh rendering with the graphics pipeline
2. Implement camera system with MVP matrices
3. Connect chunk manager to rendering system
4. Add texture atlas support
5. Optimize rendering with frustum culling
