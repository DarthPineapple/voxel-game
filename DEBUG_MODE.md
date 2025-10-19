# Debug Mode Guide

## Overview

The voxel game now includes a comprehensive debug mode that allows you to:
- Enable/disable debug logging
- Process the game frame-by-frame
- View detailed mesh information
- Inspect transformed mesh data with MVP matrices

## Usage

### Enabling Debug Mode

Press **F1** to toggle debug mode on/off.

When enabled, you'll see:
```
*** DEBUG MODE ENABLED ***
Press F2 to enable frame-by-frame stepping
Press F1 to disable debug mode
```

### Frame-by-Frame Stepping

1. First, enable debug mode by pressing **F1**
2. Press **F2** to enable frame-by-frame stepping mode
3. The game will pause and wait for you to press **F2** again to advance to the next frame
4. Each time you press **F2**, the game processes one frame and displays debug information

### Debug Information Displayed

When debug mode is active, each frame displays:

#### Frame Information
- Render time in milliseconds
- Camera position (x, y, z)
- Camera orientation (yaw and pitch)

#### Mesh Information
- Vertex count
- Index count
- Triangle count
- Vertex buffer address
- Index buffer address
- Buffer sizes in bytes
- Sample vertices (first 3 and last 3 vertices with position, normal, and UV data)

#### Transformed Mesh Information
- MVP (Model-View-Projection) matrix (4x4)
- Viewport dimensions
- Aspect ratio
- Sample transformed vertices showing:
  - Original position
  - Clip space position (4D homogeneous coordinates before perspective divide)
  - Normalized Device Coordinates (NDC) (after perspective divide)

## Example Output

```
========== DEBUG FRAME INFO ==========
[Frame] Render time: 2.34 ms
[Camera] Position: (8, 8, 20)
[Camera] Yaw: 0 Pitch: 0
[Mesh] Vertex count: 384
[Mesh] Index count: 576
[Mesh] Triangle count: 192
[Mesh] Vertex buffer: 0x7f8b4c000000
[Mesh] Index buffer: 0x7f8b4c001000
[Mesh] Vertex buffer size: 12288 bytes
[Mesh] Index buffer size: 2304 bytes
[Mesh] Sample vertices (first 3):
  Vertex 0: pos(1, 0, 0) normal(1, 0, 0) uv(0, 0)
  Vertex 1: pos(1, 1, 0) normal(1, 0, 0) uv(1, 0)
  Vertex 2: pos(1, 1, 1) normal(1, 0, 0) uv(1, 1)
[Transform] MVP Matrix:
  [1.81066, 0, 0, 0]
  [0, -2.41421, 0, 0]
  [0, 0, 1.002, 1]
  [0, 0, -0.2002, 0]
[Transform] Viewport: 800x600
[Transform] Aspect ratio: 1.33333
[Transform] Sample transformed vertices (first 3):
  Vertex 0: 
    Original: (1, 0, 0)
    Clip space: (1.81066, 0, 1.002, 1)
    NDC: (1.81066, 0, 1.002)
======================================
```

## Integration with Existing Controls

Debug mode works alongside all existing controls:
- **WASD**: Camera movement (forward, left, back, right)
- **Space/Shift**: Move up/down
- **Arrow Keys**: Camera rotation
- **F1**: Toggle debug mode
- **F2**: Enable frame stepping / advance frame (when in frame stepping mode)

## Use Cases

### Debugging Rendering Issues
Enable debug mode to inspect:
- Whether vertices are being generated correctly
- If the MVP matrix is properly transforming vertices
- Whether vertices are within the visible clip space

### Performance Analysis
Use frame-by-frame mode to:
- Analyze rendering performance per frame
- Identify expensive operations
- Debug frame timing issues

### Mesh Generation Verification
Check mesh generation by:
- Inspecting vertex positions and normals
- Verifying texture coordinates
- Confirming correct face culling

## Technical Details

### Implementation

The debug system is implemented across several components:

1. **Application** (`src/engine/application.cpp`):
   - Manages debug mode state
   - Handles keyboard input for F1/F2 keys
   - Controls frame-by-frame stepping
   - Calls debug logging methods

2. **Renderer** (`src/graphics/renderer.cpp`):
   - Provides `logMeshInfo()` method for mesh data
   - Provides `logTransformedMeshInfo()` for transformation data
   - Calculates and displays MVP matrices

3. **Mesh** (`src/graphics/mesh.cpp`):
   - Stores vertex data for debug access
   - Exposes vertex and index counts
   - Provides buffer handle information

### Memory Overhead

Debug mode stores a copy of vertex data in the Mesh class for inspection. This adds minimal memory overhead (typically a few KB to MB depending on mesh size) but does not impact rendering performance.

### Thread Safety

Debug mode is single-threaded and operates on the main render loop. All debug operations are synchronous.
