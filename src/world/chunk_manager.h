#ifndef CHUNK_MANAGER_H
#define CHUNK_MANAGER_H

#include <vector>
#include <unordered_map>
#include <tuple>
#include "chunk.h"
#include "utils/tuple_hash.h"

class ChunkManager {
public:
    ChunkManager();
    ~ChunkManager();

    void init();
    void addChunk(int x, int y, int z);
    void removeChunk(int x, int y, int z);
    void update();
    void cleanup();
    
    // Dynamic chunk loading around camera
    void updateChunksAroundCamera(float camX, float camY, float camZ, int renderDistance);
    
    // Get all active chunks
    const std::vector<Chunk*>& getChunks() const { return chunks; }
    
    // Check if a chunk exists at given position
    bool hasChunk(int x, int y, int z) const;
    
    // Get chunk at position (returns nullptr if not found)
    Chunk* getChunk(int x, int y, int z);

private:
    std::vector<Chunk*> chunks;
    std::unordered_map<std::tuple<int, int, int>, Chunk*, TupleHash> chunkMap;  // For O(1) lookup
};

#endif // CHUNK_MANAGER_H