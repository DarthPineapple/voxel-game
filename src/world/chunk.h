#ifndef CHUNK_H
#define CHUNK_H

#include <vector>
#include "voxel.h"

#define CHUNK_SIZE 16

class Chunk {
public:
    Chunk(int x, int y, int z);
    ~Chunk();

    void load();
    void unload();
    void update();

    int getPosX() const { return posX; }
    int getPosY() const { return posY; }
    int getPosZ() const { return posZ; }

    const std::vector<Voxel>& getVoxels() const;

private:
    int posX, posY, posZ;
    std::vector<Voxel> voxels;
    bool isLoaded;
    
    void generateVoxels();
};

#endif // CHUNK_H