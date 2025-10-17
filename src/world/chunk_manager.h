#ifndef CHUNK_MANAGER_H
#define CHUNK_MANAGER_H

#include <vector>
#include "chunk.h"

class ChunkManager {
public:
    ChunkManager();
    ~ChunkManager();

    void init();
    void addChunk(int x, int y, int z);
    void removeChunk(int x, int y, int z);
    void update();
    void cleanup();

private:
    std::vector<Chunk*> chunks;
};

#endif // CHUNK_MANAGER_H