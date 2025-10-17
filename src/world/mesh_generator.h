#ifndef MESH_GENERATOR_H
#define MESH_GENERATOR_H

#include <vector>
#include "chunk.h"
#include "graphics/mesh.h"

class MeshGenerator {
public:
    static void generateChunkMesh(const Chunk& chunk, 
                                  std::vector<Vertex>& vertices, 
                                  std::vector<uint32_t>& indices);

private:
    static bool isVoxelSolid(const Chunk& chunk, int x, int y, int z);
    static void addFace(std::vector<Vertex>& vertices, 
                       std::vector<uint32_t>& indices,
                       int x, int y, int z, int face);
};

#endif // MESH_GENERATOR_H
