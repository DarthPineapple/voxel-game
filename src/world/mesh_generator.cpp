#include "mesh_generator.h"

void MeshGenerator::generateChunkMesh(const Chunk& chunk, 
                                     std::vector<Vertex>& vertices, 
                                     std::vector<uint32_t>& indices) {
    vertices.clear();
    indices.clear();

    const std::vector<Voxel>& voxels = chunk.getVoxels();

    // Note: Index calculation must match chunk voxel storage order
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int y = 0; y < CHUNK_SIZE; y++) {
            for (int z = 0; z < CHUNK_SIZE; z++) {
                int index = x + y * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_SIZE;
                
                if (voxels[index].getType() == 0) continue; // Skip air voxels

                // Check each face and add geometry if exposed
                // Face 0: +X (right)
                if (x == CHUNK_SIZE - 1 || !isVoxelSolid(chunk, x + 1, y, z)) {
                    addFace(vertices, indices, x, y, z, 0);
                }
                // Face 1: -X (left)
                if (x == 0 || !isVoxelSolid(chunk, x - 1, y, z)) {
                    addFace(vertices, indices, x, y, z, 1);
                }
                // Face 2: +Y (top)
                if (y == CHUNK_SIZE - 1 || !isVoxelSolid(chunk, x, y + 1, z)) {
                    addFace(vertices, indices, x, y, z, 2);
                }
                // Face 3: -Y (bottom)
                if (y == 0 || !isVoxelSolid(chunk, x, y - 1, z)) {
                    addFace(vertices, indices, x, y, z, 3);
                }
                // Face 4: +Z (front)
                if (z == CHUNK_SIZE - 1 || !isVoxelSolid(chunk, x, y, z + 1)) {
                    addFace(vertices, indices, x, y, z, 4);
                }
                // Face 5: -Z (back)
                if (z == 0 || !isVoxelSolid(chunk, x, y, z - 1)) {
                    addFace(vertices, indices, x, y, z, 5);
                }
            }
        }
    }
}

bool MeshGenerator::isVoxelSolid(const Chunk& chunk, int x, int y, int z) {
    if (x < 0 || x >= CHUNK_SIZE || y < 0 || y >= CHUNK_SIZE || z < 0 || z >= CHUNK_SIZE) {
        return false; // Out of bounds, treat as air
    }
    
    // Index calculation matches chunk voxel storage order
    int index = x + y * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_SIZE;
    return chunk.getVoxels()[index].getType() != 0;
}

void MeshGenerator::addFace(std::vector<Vertex>& vertices, 
                           std::vector<uint32_t>& indices,
                           int x, int y, int z, int face) {
    uint32_t baseIndex = static_cast<uint32_t>(vertices.size());
    
    float fx = static_cast<float>(x);
    float fy = static_cast<float>(y);
    float fz = static_cast<float>(z);
    
    Vertex v1, v2, v3, v4;
    
    switch (face) {
        case 0: // +X (right)
            v1 = {{fx + 1, fy, fz}, {1, 0, 0}, {0, 0}};
            v2 = {{fx + 1, fy + 1, fz}, {1, 0, 0}, {1, 0}};
            v3 = {{fx + 1, fy + 1, fz + 1}, {1, 0, 0}, {1, 1}};
            v4 = {{fx + 1, fy, fz + 1}, {1, 0, 0}, {0, 1}};
            break;
        case 1: // -X (left)
            v1 = {{fx, fy, fz + 1}, {-1, 0, 0}, {0, 0}};
            v2 = {{fx, fy + 1, fz + 1}, {-1, 0, 0}, {1, 0}};
            v3 = {{fx, fy + 1, fz}, {-1, 0, 0}, {1, 1}};
            v4 = {{fx, fy, fz}, {-1, 0, 0}, {0, 1}};
            break;
        case 2: // +Y (top)
            v1 = {{fx, fy + 1, fz}, {0, 1, 0}, {0, 0}};
            v2 = {{fx, fy + 1, fz + 1}, {0, 1, 0}, {1, 0}};
            v3 = {{fx + 1, fy + 1, fz + 1}, {0, 1, 0}, {1, 1}};
            v4 = {{fx + 1, fy + 1, fz}, {0, 1, 0}, {0, 1}};
            break;
        case 3: // -Y (bottom)
            v1 = {{fx, fy, fz + 1}, {0, -1, 0}, {0, 0}};
            v2 = {{fx, fy, fz}, {0, -1, 0}, {1, 0}};
            v3 = {{fx + 1, fy, fz}, {0, -1, 0}, {1, 1}};
            v4 = {{fx + 1, fy, fz + 1}, {0, -1, 0}, {0, 1}};
            break;
        case 4: // +Z (front)
            v1 = {{fx, fy, fz + 1}, {0, 0, 1}, {0, 0}};
            v2 = {{fx + 1, fy, fz + 1}, {0, 0, 1}, {1, 0}};
            v3 = {{fx + 1, fy + 1, fz + 1}, {0, 0, 1}, {1, 1}};
            v4 = {{fx, fy + 1, fz + 1}, {0, 0, 1}, {0, 1}};
            break;
        case 5: // -Z (back)
            v1 = {{fx + 1, fy, fz}, {0, 0, -1}, {0, 0}};
            v2 = {{fx, fy, fz}, {0, 0, -1}, {1, 0}};
            v3 = {{fx, fy + 1, fz}, {0, 0, -1}, {1, 1}};
            v4 = {{fx + 1, fy + 1, fz}, {0, 0, -1}, {0, 1}};
            break;
    }
    
    vertices.push_back(v1);
    vertices.push_back(v2);
    vertices.push_back(v3);
    vertices.push_back(v4);
    
    // Two triangles per face with clockwise winding (to match pipeline front face expectation)
    // Pipeline expects VK_FRONT_FACE_CLOCKWISE due to Y-flip in projection matrix
    indices.push_back(baseIndex);
    indices.push_back(baseIndex + 2);
    indices.push_back(baseIndex + 1);
    
    indices.push_back(baseIndex);
    indices.push_back(baseIndex + 3);
    indices.push_back(baseIndex + 2);
}
