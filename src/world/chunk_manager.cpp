#include "chunk_manager.h"
#include "chunk.h"
#include <vector>
#include <algorithm>

ChunkManager::ChunkManager() {
    // Initialize chunk storage
}

ChunkManager::~ChunkManager() {
    // Cleanup chunks
    for (auto& chunk : chunks) {
        delete chunk;
    }
}

void ChunkManager::addChunk(int x, int y, int z) {
    Chunk* newChunk = new Chunk(x, y, z);
    newChunk->load();
    chunks.push_back(newChunk);
}

void ChunkManager::removeChunk(int x, int y, int z) {
    auto it = std::remove_if(chunks.begin(), chunks.end(),
        [x, y, z](Chunk* chunk) {
            return chunk->getPosition() == glm::vec3(x, y, z);
        });

    if (it != chunks.end()) {
        (*it)->unload();
        delete *it;
        chunks.erase(it, chunks.end());
    }
}

void ChunkManager::updateChunks() {
    for (auto& chunk : chunks) {
        chunk->update();
    }
}