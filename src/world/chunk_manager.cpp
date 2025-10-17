#include "chunk_manager.h"
#include "chunk.h"
#include <vector>
#include <algorithm>

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
    Chunk* newChunk = new Chunk(x, y, z);
    newChunk->load();
    chunks.push_back(newChunk);
}

void ChunkManager::removeChunk(int x, int y, int z) {
    auto it = std::remove_if(chunks.begin(), chunks.end(),
        [x, y, z](Chunk* chunk) {
            return chunk->getPosX() == x && chunk->getPosY() == y && chunk->getPosZ() == z;
        });

    if (it != chunks.end()) {
        for (auto iter = it; iter != chunks.end(); ++iter) {
            (*iter)->unload();
            delete *iter;
        }
        chunks.erase(it, chunks.end());
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
}