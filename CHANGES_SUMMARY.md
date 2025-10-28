# Changes Summary: Disable Overlay and Fix Cube Inversion

## Overview
This pull request addresses two issues:
1. Disables the overlay rendering feature (temporarily)
2. Fixes the cube face culling issue where some faces were being inverted

## Changes Made

### 1. Disabled Overlay Rendering (`src/graphics/renderer.cpp`)

**Location:** Lines 241-261 in `recordCommandBuffer()` method

**Change:** Commented out the overlay pipeline binding and draw calls with a TODO note for future re-enabling.

**Reason:** As requested, the overlay feature is disabled for now. The code is preserved as comments to allow easy re-enabling in the future.

### 2. Fixed Cube Face Culling (`src/graphics/vulkan/pipeline.cpp`)

**Location:** Line 108 in `createPipeline()` method

**Change:** Changed `rasterizer.frontFace` from `VK_FRONT_FACE_COUNTER_CLOCKWISE` to `VK_FRONT_FACE_CLOCKWISE`

**Reason:** The projection matrix in `camera.cpp` (line 124) uses a negative Y-axis scale (`-1.0f / tanHalfFov`) to convert from world space (Y-up) to Vulkan's NDC space (Y-down). This Y-axis flip inverts the winding order of triangles. Faces that are counter-clockwise in world space appear clockwise after the projection, so we need to tell Vulkan that clockwise faces are front-facing.

## Technical Explanation: Cube Inversion Issue

### Root Cause
1. **Mesh Generation:** The mesh generator creates cube faces with counter-clockwise (CCW) winding order when viewed from outside in world space (Y-up coordinate system).
2. **Projection Matrix:** The camera's projection matrix flips the Y-axis (negative scale) to convert from Y-up world space to Y-down Vulkan NDC space.
3. **Winding Order Inversion:** When you flip the Y coordinate, a CCW triangle becomes clockwise (CW).
4. **Incorrect Culling:** With `frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE` and `cullMode = VK_CULL_MODE_BACK_BIT`, Vulkan was culling the wrong faces because it thought CCW faces were front-facing, but after the Y-flip they were actually CW.

### Solution
By setting `frontFace = VK_FRONT_FACE_CLOCKWISE`, we tell Vulkan that after the projection transformation, clockwise faces should be considered front-facing. This correctly culls back faces and renders front faces.

### Verification
To verify the mesh generator is correct, I analyzed each face's winding order:
- All faces (+X, -X, +Y, -Y, +Z, -Z) have vertices ordered counter-clockwise when viewed from outside
- The cross product of edges confirms normals point outward
- The issue was purely in the pipeline configuration, not the mesh data

## Testing Instructions

### Before Testing
1. Compile shaders: `./compile_shaders.sh`
2. Build the project: `mkdir build && cd build && cmake .. && cmake --build .`
3. Run the application: `./VoxelGame`

### Expected Results

#### Overlay
- **Before:** A rotating square overlay was visible on screen
- **After:** No overlay should be visible, only the voxel chunk

#### Cube Rendering
- **Before:** Some cube faces were incorrectly culled, making the cube appear "inside-out" or showing incorrect faces depending on the camera angle
- **After:** All cube faces should render correctly:
  - Back faces (facing away from camera) are culled
  - Front faces (facing toward camera) are visible
  - The cube should look solid from all angles
  - No faces should appear inverted or inside-out

### Manual Testing Checklist
- [ ] Application starts without errors
- [ ] Voxel chunk is visible
- [ ] **No overlay is rendered**
- [ ] Cube faces are correctly oriented (not inverted)
- [ ] Moving the camera around the chunk shows correct face culling
- [ ] No faces appear "inside-out" from any angle
- [ ] Back faces are properly hidden

## Files Modified
1. `src/graphics/renderer.cpp` - Disabled overlay rendering
2. `src/graphics/vulkan/pipeline.cpp` - Fixed front face winding order

## Backward Compatibility
These changes do not break any existing functionality:
- The overlay code is preserved in comments for future use
- The face culling fix corrects a rendering bug without changing the mesh data
- No API changes or interface modifications

## Future Work
- Re-enable overlay rendering when needed by uncommenting the code in `renderer.cpp`
- Consider adding a configuration flag to toggle overlay rendering at runtime
- Potentially add depth testing if not already enabled for proper 3D rendering
