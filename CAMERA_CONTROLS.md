# Camera Controls

The voxel game now supports camera movement and rotation using keyboard input with full 3D flight controls.

## Controls

### Movement
- **W** - Move forward (in the direction you're looking, including vertical component)
- **S** - Move backward (opposite to where you're looking)
- **A** - Strafe left (perpendicular to view direction)
- **D** - Strafe right (perpendicular to view direction)
- **Space** - Move up (relative to current orientation)
- **Left Shift** - Move down (relative to current orientation)

### Rotation
- **Left Arrow** - Rotate left (yaw)
- **Right Arrow** - Rotate right (yaw)
- **Up Arrow** - Look up (pitch)
- **Down Arrow** - Look down (pitch)

## Movement System

The camera uses a **full 3D flight control system** where movement is dependent on your current view direction:

- **Forward/Backward movement** follows your exact view direction, including both horizontal (yaw) and vertical (pitch) components
- When looking up and moving forward, you will move upward and forward
- When looking down and moving forward, you will move downward and forward
- **Strafe movement** (left/right) is always perpendicular to your view direction but remains horizontal
- **Up/Down movement** (Space/Shift) is relative to your current camera orientation

## Camera Settings

The camera has the following default parameters (can be modified in `src/engine/camera.cpp`):

- **Default Constructor Position**: (0.0, 0.0, 5.0)
- **Runtime Initial Position** (set in renderer.cpp): (8.0, 8.0, 20.0)
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
- **Yaw Normalization**: Yaw angle is normalized to [-π, π] range to prevent floating-point precision issues and ensure consistent rotation behavior
- **Pitch Clamping**: Pitch angle is clamped to approximately ±88 degrees to prevent gimbal lock

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
