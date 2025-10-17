# Shader Compilation Guide

This guide explains how to compile GLSL shaders to SPIR-V format for use with Vulkan.

## Prerequisites

You need one of the following shader compilers:

### Option 1: glslangValidator (Recommended for Linux)
```bash
# Ubuntu/Debian
sudo apt-get install glslang-tools

# Arch Linux
sudo pacman -S glslang

# Fedora
sudo dnf install glslang
```

### Option 2: glslc (Part of Vulkan SDK)
Download and install the [Vulkan SDK](https://vulkan.lunarg.com/sdk/home)

## Compiling Shaders

### Using the provided script
The repository includes a `compile_shaders.sh` script that automates shader compilation:

```bash
./compile_shaders.sh
```

### Manual compilation

#### Using glslangValidator:
```bash
cd assets/shaders
glslangValidator -V shader.vert -o shader.vert.spv
glslangValidator -V shader.frag -o shader.frag.spv
```

#### Using glslc:
```bash
cd assets/shaders
glslc shader.vert -o shader.vert.spv
glslc shader.frag -o shader.frag.spv
```

## Shader Files

The project includes the following shaders:

- **shader.vert** - Vertex shader that processes vertex attributes (position, normal, texCoord)
- **shader.frag** - Fragment shader that applies simple directional lighting

After compilation, you should have:

- **shader.vert.spv** - Compiled vertex shader
- **shader.frag.spv** - Compiled fragment shader

## Important Notes

1. Shaders **must** be compiled before running the application
2. Compiled shader files (*.spv) are in .gitignore and not committed to the repository
3. You need to recompile shaders if you modify the .vert or .frag source files
4. The application loads shaders from `assets/shaders/` directory

## Troubleshooting

### "glslc not found" or "glslangValidator not found"
Install the appropriate shader compiler using the instructions above.

### "Failed to open file" at runtime
Ensure you've compiled the shaders and the .spv files exist in `assets/shaders/`.

### Compilation errors
Check that your GLSL syntax is correct. The shaders use GLSL version 450 (Vulkan).

## Shader Architecture

### Vertex Input Layout
The vertex shader expects the following input layout matching the `Vertex` struct:

```glsl
layout(location = 0) in vec3 inPosition;  // Vertex position
layout(location = 1) in vec3 inNormal;    // Surface normal
layout(location = 2) in vec2 inTexCoord;  // Texture coordinates
```

This corresponds to the C++ Vertex structure:
```cpp
struct Vertex {
    float position[3];  // 12 bytes
    float normal[3];    // 12 bytes
    float texCoord[2];  // 8 bytes
};  // Total: 32 bytes
```

### Fragment Shader
The fragment shader receives interpolated vertex attributes and applies simple directional lighting based on surface normals.
