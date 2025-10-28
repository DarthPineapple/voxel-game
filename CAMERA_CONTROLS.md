# Camera Controls

The voxel game now supports camera movement and rotation using keyboard input with FPS-style controls.

## Controls

### Movement
- **W** - Move forward on the horizontal plane (direction based on yaw rotation)
- **S** - Move backward on the horizontal plane (opposite to yaw direction)
- **A** - Strafe left (perpendicular to yaw direction)
- **D** - Strafe right (perpendicular to yaw direction)
- **Space** - Move straight up in world space
- **Left Shift** - Move straight down in world space

### Rotation
- **Left Arrow** - Rotate left (yaw)
- **Right Arrow** - Rotate right (yaw)
- **Up Arrow** - Look up (pitch)
- **Down Arrow** - Look down (pitch)

## Movement System

The camera uses an **FPS-style control system** where horizontal movement is based on yaw rotation only:

- **Forward/Backward movement** (W/S) moves on the horizontal plane in the direction of your yaw rotation, independent of pitch
- Looking up or down does NOT affect horizontal movement direction - you always move parallel to the ground
- **Strafe movement** (A/D) is always perpendicular to your yaw direction and remains horizontal
- **Up/Down movement** (Space/Shift) moves straight up or down in world space coordinates

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
- **Constant Rotation Speed**: Rotation speed is constant and frame-rate independent (standard FPS behavior)
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
