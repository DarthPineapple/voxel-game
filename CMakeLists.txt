cmake_minimum_required(VERSION 3.10)

project(VoxelGame)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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

# Include directories - use target_include_directories to maintain proper search order
target_include_directories(VoxelGame PRIVATE
    src
    src/engine
    src/graphics/vulkan
    src/world
    src/utils
)

# Link libraries
target_link_libraries(VoxelGame Vulkan::Vulkan glfw)

# Ensure this is placed after your target is created (add_executable/add_library).
# If you use a different target variable/name, adjust accordingly.
target_sources(VoxelGame PRIVATE
    src/graphics/vulkan/vulkan_instance.cpp
)