# Camera Controls

The voxel game now supports camera movement and rotation using keyboard input.

## Controls

### Movement
- **W** - Move forward
- **S** - Move backward
- **A** - Strafe left
- **D** - Strafe right
- **Space** - Move up
- **Left Shift** - Move down

### Rotation
- **Left Arrow** - Rotate left (yaw)
- **Right Arrow** - Rotate right (yaw)
- **Up Arrow** - Look up (pitch)
- **Down Arrow** - Look down (pitch)

## Camera Settings

The camera has the following default parameters (can be modified in `src/engine/camera.cpp`):

- **Initial Position**: (8.0, 8.0, 20.0)
- **Move Speed**: 3.0 units/second
- **Rotation Speed**: 1.5 radians/second
- **Field of View**: 45 degrees
- **Near Plane**: 0.1
- **Far Plane**: 100.0

## Implementation Details

The camera system uses:
- **MVP Matrix**: Model-View-Projection matrix calculated per frame
- **Uniform Buffers**: GPU buffers to pass the MVP matrix to shaders
- **Descriptor Sets**: Vulkan descriptor sets to bind uniform buffers to shaders

The vertex shader (`assets/shaders/shader.vert`) now receives the MVP matrix through a uniform buffer binding and applies it to transform vertices from world space to clip space.

## Building and Running

1. Compile the shaders:
   ```bash
   ./compile_shaders.sh
   ```

2. Build the project:
   ```bash
   mkdir build
   cd build
   cmake ..
   cmake --build .
   ```

3. Run the game:
   ```bash
   ./VoxelGame
   ```

You should now be able to move the camera around the voxel world using the keyboard controls.
