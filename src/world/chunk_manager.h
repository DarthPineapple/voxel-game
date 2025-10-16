#ifndef CHUNK_MANAGER_H
#define CHUNK_MANAGER_H

#include <vector>
#include "chunk.h"

class ChunkManager {
public:
    ChunkManager();
    ~ChunkManager();

    void addChunk(const Chunk& chunk);
    void removeChunk(int chunkId);
    void updateChunks();

private:
    std::vector<Chunk> chunks;
};

#endif // CHUNK_MANAGER_H