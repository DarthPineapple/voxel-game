#ifndef VOXEL_H
#define VOXEL_H

class Voxel {
public:
    Voxel(int x, int y, int z, int type);
    
    int getPositionX() const;
    int getPositionY() const;
    int getPositionZ() const;
    int getType() const;

private:
    int positionX;
    int positionY;
    int positionZ;
    int voxelType;
};

#endif // VOXEL_H