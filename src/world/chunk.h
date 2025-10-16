#ifndef CHUNK_H
#define CHUNK_H

#include <vector>
#include "voxel.h"

class Chunk {
public:
    Chunk(int x, int y, int z);
    ~Chunk();

    void load();
    void unload();
    void update();

    const std::vector<Voxel>& getVoxels() const;

private:
    int posX, posY, posZ;
    std::vector<Voxel> voxels;
    bool isLoaded;
};

#endif // CHUNK_H