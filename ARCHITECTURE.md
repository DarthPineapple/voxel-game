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
│   └── vulkan/      # Vulkan-specific components
│       ├── vulkan_instance  # Instance and surface creation
│       ├── device           # Physical/logical device management
│       ├── swapchain        # Swapchain creation and management
│       └── pipeline         # Graphics pipeline (placeholder)
│
├── world/           # Voxel world management
│   ├── voxel        # Individual voxel representation
│   ├── chunk        # Chunk data structure (16x16x16 voxels)
│   └── chunk_manager # Chunk loading/unloading system
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
- No single file exceeds 165 lines (previously: renderer.cpp was 351 lines)
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
        └── Swapchain (swapchain, images)
```

## Future Enhancements

Based on the roadmap in README.md:
1. Image views, render pass, framebuffers
2. Command pool/buffers and synchronization
3. Graphics pipeline and shader integration
4. Voxel mesh rendering

Each of these can be implemented as separate modules following the established pattern.
