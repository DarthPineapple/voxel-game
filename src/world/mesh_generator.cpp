#include "mesh_generator.h"
#include <cstring> // for memset
#include <algorithm> // for std::any_of

void MeshGenerator::generateChunkMesh(const Chunk& chunk, 
                                     std::vector<Vertex>& vertices, 
                                     std::vector<uint32_t>& indices) {
    vertices.clear();
    indices.clear();
    
    // Early exit optimization: check if chunk has any solid voxels
    const auto& voxels = chunk.getVoxels();
    bool hasAnyVoxel = std::any_of(voxels.begin(), voxels.end(), 
                                    [](const Voxel& v) { return v.getType() != 0; });
    
    // Skip meshing entirely if chunk is completely empty
    if (!hasAnyVoxel) {
        return;
    }
    
    // Get chunk world position offset
    int chunkOffsetX = chunk.getPosX() * CHUNK_SIZE;
    int chunkOffsetY = chunk.getPosY() * CHUNK_SIZE;
    int chunkOffsetZ = chunk.getPosZ() * CHUNK_SIZE;

    // Apply greedy meshing for each of the three axes
    // axis 0: X-axis (generates faces perpendicular to X)
    // axis 1: Y-axis (generates faces perpendicular to Y)
    // axis 2: Z-axis (generates faces perpendicular to Z)
    for (int axis = 0; axis < 3; ++axis) {
        greedyMeshAxis(chunk, vertices, indices, axis, chunkOffsetX, chunkOffsetY, chunkOffsetZ);
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

uint8_t MeshGenerator::getVoxelType(const Chunk& chunk, int x, int y, int z) {
    if (x < 0 || x >= CHUNK_SIZE || y < 0 || y >= CHUNK_SIZE || z < 0 || z >= CHUNK_SIZE) {
        return 0; // Out of bounds, treat as air
    }
    
    int index = x + y * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_SIZE;
    return chunk.getVoxels()[index].getType();
}

void MeshGenerator::greedyMeshAxis(const Chunk& chunk,
                                   std::vector<Vertex>& vertices,
                                   std::vector<uint32_t>& indices,
                                   int axis,
                                   int chunkOffsetX, int chunkOffsetY, int chunkOffsetZ) {
    // For greedy meshing, we sweep through slices perpendicular to the axis
    // and merge adjacent faces with the same voxel type
    
    // axis 0 = X, axis 1 = Y, axis 2 = Z
    // u and v are the two axes perpendicular to the main axis
    int u = (axis + 1) % 3;
    int v = (axis + 2) % 3;
    
    // x holds the coordinates of the current voxel
    int x[3] = {0, 0, 0};
    
    // mask to track which voxel faces are exposed in current slice
    // we use voxel type as the mask value (0 = no face, >0 = face with that type)
    uint8_t mask[CHUNK_SIZE * CHUNK_SIZE];
    
    // We iterate over each slice perpendicular to the axis
    for (x[axis] = -1; x[axis] < CHUNK_SIZE;) {
        // Clear the mask
        std::memset(mask, 0, sizeof(mask));
        
        // Build the mask for this slice
        for (x[v] = 0; x[v] < CHUNK_SIZE; ++x[v]) {
            for (x[u] = 0; x[u] < CHUNK_SIZE; ++x[u]) {
                // Get voxel types on both sides of the slice
                // voxelType1 is the voxel at current position
                // voxelType2 is the voxel in the +axis direction
                uint8_t voxelType1 = (x[axis] >= 0) ? getVoxelType(chunk, x[0], x[1], x[2]) : 0;
                
                int x2[3] = {x[0], x[1], x[2]};
                x2[axis]++;
                uint8_t voxelType2 = (x2[axis] < CHUNK_SIZE) ? getVoxelType(chunk, x2[0], x2[1], x2[2]) : 0;
                
                // If the voxels are different, we have an exposed face
                // We store the type of the solid voxel in the mask
                if (voxelType1 != 0 && voxelType2 == 0) {
                    // Face pointing in positive axis direction
                    mask[x[u] + x[v] * CHUNK_SIZE] = voxelType1;
                } else if (voxelType1 == 0 && voxelType2 != 0) {
                    // Face pointing in negative axis direction
                    // We use bit 7 to indicate back faces
                    mask[x[u] + x[v] * CHUNK_SIZE] = voxelType2 | 0x80;
                }
            }
        }
        
        ++x[axis];
        
        // Generate mesh from the mask using greedy meshing
        int n = 0;
        for (int j = 0; j < CHUNK_SIZE; ++j) {
            for (int i = 0; i < CHUNK_SIZE;) {
                if (mask[n] != 0) {
                    uint8_t currentMask = mask[n];
                    bool backFace = (currentMask & 0x80) != 0;
                    
                    // Compute width (expand in u direction)
                    int width;
                    for (width = 1; i + width < CHUNK_SIZE && mask[n + width] == currentMask; ++width) {}
                    
                    // Compute height (expand in v direction)
                    int height;
                    bool done = false;
                    for (height = 1; j + height < CHUNK_SIZE; ++height) {
                        // Check if the entire row matches
                        for (int k = 0; k < width; ++k) {
                            if (mask[n + k + height * CHUNK_SIZE] != currentMask) {
                                done = true;
                                break;
                            }
                        }
                        if (done) break;
                    }
                    
                    // Set up base coordinates for the quad
                    int quadPos[3];
                    quadPos[axis] = x[axis];
                    quadPos[u] = i;
                    quadPos[v] = j;
                    
                    // Create size parameters based on u,v axes
                    int quadWidth = width;  // extends in u direction
                    int quadHeight = height; // extends in v direction
                    
                    addQuad(vertices, indices,
                           quadPos[0], quadPos[1], quadPos[2],
                           quadWidth, quadHeight,
                           axis, backFace,
                           chunkOffsetX, chunkOffsetY, chunkOffsetZ);
                    
                    // Clear the mask in the merged region
                    for (int l = 0; l < height; ++l) {
                        for (int k = 0; k < width; ++k) {
                            mask[n + k + l * CHUNK_SIZE] = 0;
                        }
                    }
                    
                    // Move forward by the width we just processed
                    i += width;
                    n += width;
                } else {
                    ++i;
                    ++n;
                }
            }
        }
    }
}

void MeshGenerator::addQuad(std::vector<Vertex>& vertices,
                           std::vector<uint32_t>& indices,
                           int x, int y, int z,
                           int width, int height,
                           int axis, bool backFace,
                           int chunkOffsetX, int chunkOffsetY, int chunkOffsetZ) {
    uint32_t baseIndex = static_cast<uint32_t>(vertices.size());
    
    // Apply chunk offset to get world coordinates
    float fx = static_cast<float>(x + chunkOffsetX);
    float fy = static_cast<float>(y + chunkOffsetY);
    float fz = static_cast<float>(z + chunkOffsetZ);
    
    Vertex v1, v2, v3, v4;
    
    // Generate quad based on axis and direction
    // axis 0 = X, axis 1 = Y, axis 2 = Z
    // backFace = true means the face points in negative axis direction
    // width extends in the u direction, height in the v direction
    // u = (axis + 1) % 3, v = (axis + 2) % 3
    
    if (axis == 0) { // X-axis faces (perpendicular to X, lying in YZ plane)
        // u = 1 (Y), v = 2 (Z)
        // width extends in Y direction, height in Z direction
        if (!backFace) { // +X face (right)
            v1 = {{fx, fy, fz}, {1, 0, 0}, {0, 0}};
            v2 = {{fx, fy + width, fz}, {1, 0, 0}, {static_cast<float>(width), 0}};
            v3 = {{fx, fy + width, fz + height}, {1, 0, 0}, {static_cast<float>(width), static_cast<float>(height)}};
            v4 = {{fx, fy, fz + height}, {1, 0, 0}, {0, static_cast<float>(height)}};
        } else { // -X face (left)
            v1 = {{fx, fy, fz + height}, {-1, 0, 0}, {0, 0}};
            v2 = {{fx, fy + width, fz + height}, {-1, 0, 0}, {static_cast<float>(width), 0}};
            v3 = {{fx, fy + width, fz}, {-1, 0, 0}, {static_cast<float>(width), static_cast<float>(height)}};
            v4 = {{fx, fy, fz}, {-1, 0, 0}, {0, static_cast<float>(height)}};
        }
    } else if (axis == 1) { // Y-axis faces (perpendicular to Y, lying in XZ plane)
        // u = 2 (Z), v = 0 (X)
        // width extends in Z direction, height in X direction
        if (!backFace) { // +Y face (top)
            v1 = {{fx, fy, fz}, {0, 1, 0}, {0, 0}};
            v2 = {{fx, fy, fz + width}, {0, 1, 0}, {static_cast<float>(width), 0}};
            v3 = {{fx + height, fy, fz + width}, {0, 1, 0}, {static_cast<float>(width), static_cast<float>(height)}};
            v4 = {{fx + height, fy, fz}, {0, 1, 0}, {0, static_cast<float>(height)}};
        } else { // -Y face (bottom)
            v1 = {{fx, fy, fz + width}, {0, -1, 0}, {0, 0}};
            v2 = {{fx, fy, fz}, {0, -1, 0}, {static_cast<float>(width), 0}};
            v3 = {{fx + height, fy, fz}, {0, -1, 0}, {static_cast<float>(width), static_cast<float>(height)}};
            v4 = {{fx + height, fy, fz + width}, {0, -1, 0}, {0, static_cast<float>(height)}};
        }
    } else { // axis == 2, Z-axis faces (perpendicular to Z, lying in XY plane)
        // u = 0 (X), v = 1 (Y)
        // width extends in X direction, height in Y direction
        if (!backFace) { // +Z face (front)
            v1 = {{fx, fy, fz}, {0, 0, 1}, {0, 0}};
            v2 = {{fx + width, fy, fz}, {0, 0, 1}, {static_cast<float>(width), 0}};
            v3 = {{fx + width, fy + height, fz}, {0, 0, 1}, {static_cast<float>(width), static_cast<float>(height)}};
            v4 = {{fx, fy + height, fz}, {0, 0, 1}, {0, static_cast<float>(height)}};
        } else { // -Z face (back)
            v1 = {{fx + width, fy, fz}, {0, 0, -1}, {0, 0}};
            v2 = {{fx, fy, fz}, {0, 0, -1}, {static_cast<float>(width), 0}};
            v3 = {{fx, fy + height, fz}, {0, 0, -1}, {static_cast<float>(width), static_cast<float>(height)}};
            v4 = {{fx + width, fy + height, fz}, {0, 0, -1}, {0, static_cast<float>(height)}};
        }
    }
    
    vertices.push_back(v1);
    vertices.push_back(v2);
    vertices.push_back(v3);
    vertices.push_back(v4);
    
    // Two triangles per quad with clockwise winding
    // Pipeline expects VK_FRONT_FACE_CLOCKWISE due to Y-flip in projection matrix
    indices.push_back(baseIndex);
    indices.push_back(baseIndex + 2);
    indices.push_back(baseIndex + 1);
    
    indices.push_back(baseIndex);
    indices.push_back(baseIndex + 3);
    indices.push_back(baseIndex + 2);
}
