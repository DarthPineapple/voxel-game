#ifndef CHUNK_MANAGER_H
#define CHUNK_MANAGER_H

#include <vector>
#include <unordered_map>
#include <tuple>
#include "chunk.h"

// Hash function for std::tuple<int, int, int> to use with unordered_map
struct TupleHash {
    std::size_t operator()(const std::tuple<int, int, int>& t) const {
        // Combine hash values of the three integers
        auto h1 = std::hash<int>{}(std::get<0>(t));
        auto h2 = std::hash<int>{}(std::get<1>(t));
        auto h3 = std::hash<int>{}(std::get<2>(t));
        return h1 ^ (h2 << 1) ^ (h3 << 2);
    }
};

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