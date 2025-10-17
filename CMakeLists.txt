cmake_minimum_required(VERSION 3.10)

project(VoxelGame)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(
    src
    src/engine
    src/graphics/vulkan
    src/world
    src/utils
)

# Source files
file(GLOB_RECURSE SOURCES
    src/*.cpp
)

# Vulkan library
find_package(Vulkan REQUIRED)

# GLFW library
find_package(glfw3 REQUIRED)

# Add executable
add_executable(VoxelGame ${SOURCES})

# Link libraries
target_link_libraries(VoxelGame Vulkan::Vulkan glfw)