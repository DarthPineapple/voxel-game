#!/bin/bash

# Script to compile GLSL shaders to SPIR-V
# Requires glslc (part of the Vulkan SDK)

set -e  # Exit on error

SHADER_DIR="assets/shaders"

# Check if glslc is available
if ! command -v glslc &> /dev/null; then
    echo "Error: glslc not found. Please install the Vulkan SDK."
    exit 1
fi

# Compile vertex shader
echo "Compiling vertex shader..."
if ! glslc "$SHADER_DIR/shader.vert" -o "$SHADER_DIR/shader.vert.spv"; then
    echo "Error: Failed to compile vertex shader"
    exit 1
fi

# Compile fragment shader
echo "Compiling fragment shader..."
if ! glslc "$SHADER_DIR/shader.frag" -o "$SHADER_DIR/shader.frag.spv"; then
    echo "Error: Failed to compile fragment shader"
    exit 1
fi

echo "Shader compilation complete!"
