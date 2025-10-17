# Voxel Game README

## Project Overview
This project is an efficient block-based voxel game that utilizes cubic chunks and dynamic chunk loading. The graphics are rendered using Vulkan, providing high performance and flexibility.

## Current Status
- ✅ Vulkan instance, surface, physical device, logical device, and swapchain creation
- ✅ Image views for swapchain images
- ✅ Render pass and framebuffers
- ✅ Command pool and command buffers
- ✅ Synchronization objects (semaphores and fences)
- ✅ Graphics pipeline with shader loading support
- ✅ Basic render loop with clear operation
- ✅ Mesh generation system for voxel chunks
- ✅ Vertex and index buffer creation
- macOS (MoltenVK) portability enumeration and subset extension handling are in place.

## Features
- Block-based voxel world
- Cubic chunk management
- Dynamic loading and unloading of chunks
- Vulkan graphics rendering

## Prerequisites
Before you begin, ensure you have the following installed:
- CMake (version 3.10 or higher)
- Vulkan SDK (1.2+; 1.3 recommended)
- GLFW (3.3+ with Vulkan support)
- A C++ compiler (e.g., GCC, Clang, MSVC)
- macOS only: MoltenVK (bundled with Vulkan SDK). Portability enumeration is enabled in code.

## Step-by-Step Checklist

### 1. Clone the Repository
Clone the project repository to your local machine:
```
git clone <repository-url>
cd voxel-game
```

### 2. Install Dependencies
- Install the Vulkan SDK for your OS.
- Install GLFW (package manager or build from source with Vulkan enabled).
- macOS: ensure MoltenVK is installed and available via the Vulkan SDK.

### 3. Build the Project
Navigate to the project directory and compile the shaders first:
```
./compile_shaders.sh
```

Then create a build directory and compile the project:
```
mkdir build
cd build
cmake ..
cmake --build .
```

### 4. Run the Application
After building the project, you can run the application:
```
./voxel-game
```

### 5. Modify and Extend
You can modify the game by editing the following files:
- **src/world/chunk.cpp** and **src/world/chunk.h**: Modify chunk behavior.
- **src/world/voxel.cpp** and **src/world/voxel.h**: Change voxel properties.
- **src/graphics/renderer.cpp** and **src/graphics/renderer.h**: Adjust rendering logic.

### 6. Add Assets
To add new textures or shaders, place them in the `assets/textures` or `assets/shaders` directories, respectively. Update the rendering code to utilize these assets.

### Platform Notes
- macOS (MoltenVK):
  - Instance uses VK_KHR_portability_enumeration and the enumerate portability flag.
  - Device enables VK_KHR_portability_subset when available.
  - Requires macOS 11+ and a compatible GPU.
- Linux:
  - Ensure Vulkan loader/ICD and GPU drivers are installed (e.g., mesa-vulkan-drivers).
- Windows:
  - Install latest GPU drivers and the Vulkan SDK.

## Troubleshooting
- Compile error: VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME undefined
  - Update Vulkan headers/SDK; a fallback macro is defined in the code.
- vkCreateInstance failed
  - Ensure glfwGetRequiredInstanceExtensions returns non-empty; update GLFW and Vulkan SDK.
- No surface formats/present modes
  - Ensure the window is created, visible, and not minimized; update GPU drivers.

## Roadmap
- ✅ Fix compilation errors on arm64 macos
- ✅ Image views, render pass, framebuffers
- ✅ Command pool/buffers and synchronization
- ✅ Graphics pipeline and shader integration
- ✅ Basic clear/draw render loop
- ✅ Voxel mesh generation system
- 🔄 Integrate mesh rendering with graphics pipeline
- 🔄 Camera system and MVP matrices
- 🔄 Chunk manager integration with rendering
- 🔄 Texture atlas support

### 7. Contribute
If you would like to contribute to the project, please fork the repository and submit a pull request with your changes.

## License
This project is licensed under the MIT License. See the LICENSE file for more details.

## Acknowledgments
- Vulkan SDK for graphics rendering
- CMake for build management

## Contact
For any questions or issues, please contact the project maintainer at [your-email@example.com].