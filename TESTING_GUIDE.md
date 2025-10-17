# Testing Guide for Camera Implementation

## Prerequisites

Before testing, ensure you have:
1. Compiled shaders by running `./compile_shaders.sh`
2. Built the project successfully
3. A working Vulkan runtime environment

## Manual Testing Checklist

### Basic Functionality
- [ ] Application starts without errors
- [ ] Window displays with voxel chunk visible
- [ ] Camera is positioned at (8.0, 8.0, 20.0) initially

### Movement Controls
- [ ] **W key**: Camera moves forward (decreasing Z in camera space)
- [ ] **S key**: Camera moves backward (increasing Z in camera space)
- [ ] **A key**: Camera strafes left
- [ ] **D key**: Camera strafes right
- [ ] **Space**: Camera moves up (increasing Y)
- [ ] **Left Shift**: Camera moves down (decreasing Y)
- [ ] Movement is smooth and responsive
- [ ] Multiple keys can be pressed simultaneously

### Rotation Controls
- [ ] **Left Arrow**: Camera rotates left (yaw)
- [ ] **Right Arrow**: Camera rotates right (yaw)
- [ ] **Up Arrow**: Camera looks up (pitch)
- [ ] **Down Arrow**: Camera looks down (pitch)
- [ ] Rotation is smooth
- [ ] Pitch is clamped to prevent looking completely up or down
- [ ] Multiple rotation keys can be pressed simultaneously

### Rendering
- [ ] Voxel chunk remains visible during movement
- [ ] Perspective projection is correct (objects appear smaller in distance)
- [ ] No graphical artifacts or flickering
- [ ] Lighting/shading updates correctly with camera movement
- [ ] No crashes or Vulkan validation errors

### Integration
- [ ] Camera updates are frame-rate independent
- [ ] Performance is acceptable (no stuttering)
- [ ] Memory leaks are absent (use Vulkan validation layers)

## Expected Behavior

1. **Initial View**: The voxel chunk should be visible in the center of the screen
2. **Movement**: Should feel like a first-person camera in 3D space
3. **Rotation**: Camera should rotate around its own position
4. **Combined Input**: Movement and rotation should work together smoothly

## Debugging

If issues occur:

1. **Shader Compilation Errors**: 
   - Check that `assets/shaders/shader.vert.spv` and `assets/shaders/shader.frag.spv` exist
   - Verify shader compilation output for errors

2. **Rendering Issues**:
   - Enable Vulkan validation layers
   - Check console output for Vulkan errors
   - Verify uniform buffer is being updated each frame

3. **Input Not Working**:
   - Verify window has focus
   - Check GLFW initialization
   - Add debug prints in `Application::processInput()`

4. **Camera Behavior Issues**:
   - Verify MVP matrix calculation in `Camera::getMVPMatrix()`
   - Check aspect ratio calculation
   - Ensure deltaTime is calculated correctly

## Code Validation

The implementation includes:
- Camera class with MVP matrix generation
- Uniform buffer creation and updates
- Descriptor sets for shader bindings
- Keyboard input processing
- Frame-rate independent movement

All file paths use the robust path resolution in `pipeline.cpp` which searches:
1. Relative path as provided
2. Under `VOXEL_GAME_ROOT` environment variable if set
3. Current working directory and parent directories

This ensures shaders can be found regardless of where the executable is run from.
