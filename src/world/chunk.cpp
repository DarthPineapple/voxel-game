#include "chunk.h"
#include <vector>

Chunk::Chunk(int x, int y, int z) : posX(x), posY(y), posZ(z), isLoaded(false) {
    // Initialize voxel data
    voxels.resize(CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE);
}

Chunk::~Chunk() {
    unload();
}

void Chunk::load() {
    if (!isLoaded) {
        // Load voxel data from disk or generate procedurally
        generateVoxels();
        isLoaded = true;
    }
}

void Chunk::unload() {
    if (isLoaded) {
        // Clean up voxel data
        voxels.clear();
        isLoaded = false;
    }
}

void Chunk::update() {
    if (isLoaded) {
        // Update voxel states or perform any necessary calculations
    }
}

const std::vector<Voxel>& Chunk::getVoxels() const {
    return voxels;
}

void Chunk::generateVoxels() {
    for (int x = 0; x < CHUNK_SIZE; ++x) {
        for (int y = 0; y < CHUNK_SIZE; ++y) {
            for (int z = 0; z < CHUNK_SIZE; ++z) {
                // Example voxel generation logic
                int index = x + y * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_SIZE;
                voxels[index] = Voxel(x, y, z, 1); // Set default type
            }
        }
    }
}