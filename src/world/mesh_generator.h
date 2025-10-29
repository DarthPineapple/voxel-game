#ifndef MESH_GENERATOR_H
#define MESH_GENERATOR_H

#include <vector>
#include "chunk.h"
#include "graphics/mesh.h"

// Mesh generator for voxel chunks using greedy meshing algorithm
// 
// Usage example:
//   Chunk chunk(0, 0, 0);
//   chunk.load();  // Generate voxel data
//   
//   std::vector<Vertex> vertices;
//   std::vector<uint32_t> indices;
//   MeshGenerator::generateChunkMesh(chunk, vertices, indices);
//   
//   Mesh* mesh = new Mesh(device, physicalDevice);
//   mesh->createVertexBuffer(vertices);
//   mesh->createIndexBuffer(indices);

class MeshGenerator {
public:
    static void generateChunkMesh(const Chunk& chunk, 
                                  std::vector<Vertex>& vertices, 
                                  std::vector<uint32_t>& indices);

private:
    static bool isVoxelSolid(const Chunk& chunk, int x, int y, int z);
    static uint8_t getVoxelType(const Chunk& chunk, int x, int y, int z);
    
    // Optimized version that takes voxels reference directly (avoids repeated getVoxels() calls)
    static inline uint8_t getVoxelTypeDirect(const std::vector<Voxel>& voxels, int x, int y, int z) {
        if (x < 0 || x >= CHUNK_SIZE || y < 0 || y >= CHUNK_SIZE || z < 0 || z >= CHUNK_SIZE) {
            return 0;
        }
        return voxels[x + y * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_SIZE].getType();
    }
    
    // Greedy meshing implementation for each axis
    static void greedyMeshAxis(const Chunk& chunk,
                               std::vector<Vertex>& vertices,
                               std::vector<uint32_t>& indices,
                               int axis,
                               int chunkOffsetX, int chunkOffsetY, int chunkOffsetZ);
    
    // Add a merged quad to the mesh
    static void addQuad(std::vector<Vertex>& vertices,
                       std::vector<uint32_t>& indices,
                       int x, int y, int z,
                       int width, int height,
                       int axis, bool backFace,
                       int chunkOffsetX, int chunkOffsetY, int chunkOffsetZ);
};

#endif // MESH_GENERATOR_H
