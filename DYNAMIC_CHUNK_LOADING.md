# Dynamic Chunk Loading System

## Overview

The voxel game now features dynamic chunk generation, loading, and meshing around the camera position. As the player moves through the world, chunks are automatically loaded and unloaded based on their distance from the camera.

## Features

- **Automatic chunk loading**: Chunks within the render distance are automatically generated and loaded
- **Automatic chunk unloading**: Chunks beyond the render distance are automatically unloaded to save memory
- **Spherical render distance**: Chunks are loaded in a sphere around the camera, not a cube
- **Efficient mesh management**: Meshes are automatically created for new chunks and cleaned up when chunks are unloaded
- **Configurable render distance**: The number of chunks to load around the camera can be adjusted

## How It Works

### Chunk Coordinates

The world is divided into chunks, where each chunk is 16x16x16 voxels. The camera position is converted to chunk coordinates:

```cpp
int chunkX = std::floor(cameraX / 16)
int chunkY = std::floor(cameraY / 16)
int chunkZ = std::floor(cameraZ / 16)
```

### Render Distance

The render distance is specified in chunks. For example, a render distance of 2 means:
- Chunks within 2 chunks of the camera are loaded
- Chunks beyond 3 chunks (renderDistance + 1 buffer) are unloaded

The buffer prevents "chunk thrashing" where chunks at the boundary are constantly loaded and unloaded.

### Spherical Loading

Rather than loading all chunks in a cube, the system calculates the actual distance to each chunk and only loads those within the spherical render distance:

```cpp
distance = sqrt(dx² + dy² + dz²)
if (distance <= renderDistance) {
    load chunk
}
```

This provides more uniform chunk loading in all directions.

## Configuration

The render distance is configured in `src/engine/application.cpp`:

```cpp
chunkManager->updateChunksAroundCamera(
    camera->getPosX(), 
    camera->getPosY(), 
    camera->getPosZ(), 
    10  // render distance in chunks
);
```

You can modify the last parameter to change the render distance:
- **1**: Very close (27 chunks maximum)
- **2**: Close (approximately 33 chunks)
- **3**: Medium (approximately 123 chunks)
- **4**: Far (approximately 257 chunks)
- **5+**: Very far (memory intensive)
- **10**: Extended range (approximately 4,189 chunks, current setting)

## Performance Considerations

### Memory Usage

Each chunk contains:
- Voxel data: 16x16x16 = 4,096 voxels
- Mesh data: Variable, depends on visible faces
- Vertex buffer: GPU memory
- Index buffer: GPU memory

With render distance 2, expect approximately 33 chunks to be loaded simultaneously.

### Frame Performance

Chunk updates occur every frame:
1. Calculate camera chunk position
2. Check which chunks should be loaded
3. Add new chunks if needed
4. Mark distant chunks for removal
5. Update renderer mesh list
6. Create GPU buffers for new chunks
7. Clean up GPU buffers for removed chunks

Most frames, no chunks are added or removed, making the operation very fast (O(1)).

### Optimization Tips

1. **Increase render distance gradually**: Test performance before setting a high render distance
2. **Profile mesh generation**: The greedy meshing algorithm is efficient but can be slow for complex geometry
3. **Monitor GPU memory**: Each chunk mesh requires GPU buffer allocation
4. **Consider async loading**: For very large render distances, consider loading chunks in a background thread

## Implementation Details

### ChunkManager

The `ChunkManager` class handles chunk lifecycle:

```cpp
class ChunkManager {
    void updateChunksAroundCamera(float camX, float camY, float camZ, int renderDistance);
    bool hasChunk(int x, int y, int z) const;
    Chunk* getChunk(int x, int y, int z);
    const std::vector<Chunk*>& getChunks() const;
};
```

Key features:
- Fast O(1) chunk lookup using `std::unordered_map`
- Duplicate prevention
- Automatic cleanup

### Renderer Integration

The `Renderer` class manages chunk meshes:

```cpp
class Renderer {
    void updateChunkMeshes(ChunkManager* chunkManager);
    Mesh* createMeshForChunk(Chunk* chunk);
};
```

The renderer:
1. Syncs with the chunk manager each frame
2. Creates meshes for newly loaded chunks
3. Destroys meshes for unloaded chunks
4. Renders all active chunk meshes

### Mesh Generation

Each chunk is converted to a mesh using the greedy meshing algorithm:
- Culls hidden faces (faces between solid voxels)
- Merges adjacent faces to reduce polygon count
- Generates vertex and index buffers
- Uploads to GPU

## Testing

To verify dynamic chunk loading:

1. **Run the application**
2. **Move the camera** using WASD keys
3. **Observe chunk loading** in the console output (if verbose logging enabled)
4. **Monitor frame time** to ensure performance is acceptable
5. **Check memory usage** doesn't grow unbounded

## Debug Mode

Enable debug mode (F1) to see:
- Current camera position
- Chunk coordinate calculations
- Mesh statistics for each frame

## Future Enhancements

Potential improvements to the dynamic chunk loading system:

- [ ] Async chunk loading in background threads
- [ ] Chunk prioritization (load closer chunks first)
- [ ] Save/load chunks to disk for persistence
- [ ] Level of detail (LOD) for distant chunks
- [ ] Chunk border matching to prevent seams
- [ ] Frustum culling (don't render chunks behind the camera)
- [ ] Occlusion culling (don't render chunks behind other chunks)

## Troubleshooting

### Chunks not loading
- Check that camera position is updating correctly
- Verify render distance > 0
- Ensure `updateChunksAroundCamera()` is called each frame

### Performance issues
- Reduce render distance
- Profile mesh generation time
- Check for GPU memory limits
- Enable frustum culling when implemented

### Visual artifacts
- Check for chunk coordinate calculation errors
- Verify mesh generation produces valid geometry
- Ensure proper cleanup of old meshes

## Code References

- **ChunkManager**: `src/world/chunk_manager.h`, `src/world/chunk_manager.cpp`
- **Renderer**: `src/graphics/renderer.h`, `src/graphics/renderer.cpp`
- **Application**: `src/engine/application.cpp`
- **Mesh Generation**: `src/world/mesh_generator.h`, `src/world/mesh_generator.cpp`
