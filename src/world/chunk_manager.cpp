#include "chunk_manager.h"
#include "chunk.h"
#include "noise.h"
#include <vector>
#include <unordered_map>
#include <tuple>
#include <algorithm>
#include <cmath>

ChunkManager::ChunkManager() {
    // Initialize chunk storage
}

ChunkManager::~ChunkManager() {
    cleanup();
}

void ChunkManager::init() {
    // Initialize the chunk manager
    // Could add some initial chunks here if needed
}

void ChunkManager::addChunk(int x, int y, int z) {
    // Check if chunk already exists
    if (hasChunk(x, y, z)) {
        return;
    }
    
    Chunk* newChunk = new Chunk(x, y, z);
    newChunk->load();
    chunks.push_back(newChunk);
    chunkMap[std::make_tuple(x, y, z)] = newChunk;
}

void ChunkManager::removeChunk(int x, int y, int z) {
    auto key = std::make_tuple(x, y, z);
    auto mapIt = chunkMap.find(key);
    
    if (mapIt != chunkMap.end()) {
        Chunk* chunk = mapIt->second;
        
        // Remove from vector
        auto it = std::remove(chunks.begin(), chunks.end(), chunk);
        chunks.erase(it, chunks.end());
        
        // Remove from map
        chunkMap.erase(mapIt);
        
        // Clean up chunk
        chunk->unload();
        delete chunk;
    }
}

void ChunkManager::update() {
    for (auto& chunk : chunks) {
        chunk->update();
    }
}

void ChunkManager::cleanup() {
    for (auto& chunk : chunks) {
        chunk->unload();
        delete chunk;
    }
    chunks.clear();
    chunkMap.clear();
}

bool ChunkManager::hasChunk(int x, int y, int z) const {
    auto key = std::make_tuple(x, y, z);
    return chunkMap.find(key) != chunkMap.end();
}

Chunk* ChunkManager::getChunk(int x, int y, int z) {
    auto key = std::make_tuple(x, y, z);
    auto it = chunkMap.find(key);
    return (it != chunkMap.end()) ? it->second : nullptr;
}

void ChunkManager::updateChunksAroundCamera(float camX, float camY, float camZ, int renderDistance) {
    // Convert camera position to chunk coordinates
    int camChunkX = static_cast<int>(std::floor(camX / CHUNK_SIZE));
    int camChunkY = static_cast<int>(std::floor(camY / CHUNK_SIZE));
    int camChunkZ = static_cast<int>(std::floor(camZ / CHUNK_SIZE));
    
    // Load chunks within render distance
    for (int x = camChunkX - renderDistance; x <= camChunkX + renderDistance; ++x) {
        for (int y = camChunkY - renderDistance; y <= camChunkY + renderDistance; ++y) {
            for (int z = camChunkZ - renderDistance; z <= camChunkZ + renderDistance; ++z) {
                // Calculate distance to chunk
                int dx = x - camChunkX;
                int dy = y - camChunkY;
                int dz = z - camChunkZ;
                float distance = std::sqrt(static_cast<float>(dx*dx + dy*dy + dz*dz));
                
                // Only load chunks within the spherical render distance
                if (distance <= renderDistance) {
                    addChunk(x, y, z);
                }
            }
        }
    }
    
    // Unload chunks outside render distance
    std::vector<std::tuple<int, int, int>> chunksToUnload;
    for (const auto& chunk : chunks) {
        int dx = chunk->getPosX() - camChunkX;
        int dy = chunk->getPosY() - camChunkY;
        int dz = chunk->getPosZ() - camChunkZ;
        float distance = std::sqrt(static_cast<float>(dx*dx + dy*dy + dz*dz));
        
        // Unload chunks beyond render distance (with small buffer to prevent thrashing)
        if (distance > renderDistance + 1) {
            chunksToUnload.push_back(std::make_tuple(chunk->getPosX(), chunk->getPosY(), chunk->getPosZ()));
        }
    }
    
    // Remove chunks that are too far
    for (const auto& chunkPos : chunksToUnload) {
        removeChunk(std::get<0>(chunkPos), std::get<1>(chunkPos), std::get<2>(chunkPos));
    }
}

float ChunkManager::getTerrainHeightAt(float worldX, float worldZ) const {
    // Use the same noise generator and parameters as chunk generation
    static PerlinNoise noise(12345);
    
    const float scale = 0.05f;
    const int octaves = 4;
    const float persistence = 0.5f;
    const float heightMultiplier = 20.0f;
    const int baseHeight = 32;
    
    // Generate height using octave noise
    float noiseValue = noise.octaveNoise(worldX * scale, worldZ * scale, octaves, persistence);
    
    // Convert noise value to terrain height
    float terrainHeight = baseHeight + noiseValue * heightMultiplier;
    
    return terrainHeight;
}