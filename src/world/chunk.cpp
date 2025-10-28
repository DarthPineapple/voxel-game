#include "chunk.h"
#include "noise.h"
#include "terrain_config.h"
#include <vector>
#include <cmath>

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
    // Create noise generator with a fixed seed for consistent terrain
    static PerlinNoise noise(TerrainConfig::NOISE_SEED);
    
    for (int x = 0; x < CHUNK_SIZE; ++x) {
        for (int z = 0; z < CHUNK_SIZE; ++z) {
            // Calculate world coordinates
            float worldX = posX * CHUNK_SIZE + x;
            float worldZ = posZ * CHUNK_SIZE + z;
            
            // Generate height using octave noise (apply scale during sampling)
            float noiseValue = noise.octaveNoise(worldX * TerrainConfig::SCALE, 
                                                  worldZ * TerrainConfig::SCALE,
                                                  TerrainConfig::OCTAVES, 
                                                  TerrainConfig::PERSISTENCE);
            
            // Convert noise value from [-1, 1] to terrain height
            int terrainHeight = TerrainConfig::BASE_HEIGHT + 
                               static_cast<int>(noiseValue * TerrainConfig::HEIGHT_MULTIPLIER);
            
            // Fill voxels based on height
            for (int y = 0; y < CHUNK_SIZE; ++y) {
                int worldY = posY * CHUNK_SIZE + y;
                int index = x + y * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_SIZE;
                
                // Set voxel type based on height
                if (worldY < terrainHeight) {
                    // Solid terrain
                    if (worldY == terrainHeight - 1) {
                        voxels[index] = Voxel(x, y, z, 1); // Grass/top layer
                    } else if (worldY >= terrainHeight - 4) {
                        voxels[index] = Voxel(x, y, z, 2); // Dirt layer
                    } else {
                        voxels[index] = Voxel(x, y, z, 3); // Stone layer
                    }
                } else {
                    // Air
                    voxels[index] = Voxel(x, y, z, 0);
                }
            }
        }
    }
}