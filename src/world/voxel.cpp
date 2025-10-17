#include "voxel.h"

Voxel::Voxel()
    : positionX(0), positionY(0), positionZ(0), voxelType(0) {}

Voxel::Voxel(int x, int y, int z, int type)
    : positionX(x), positionY(y), positionZ(z), voxelType(type) {}

int Voxel::getPositionX() const {
    return positionX;
}

int Voxel::getPositionY() const {
    return positionY;
}

int Voxel::getPositionZ() const {
    return positionZ;
}

int Voxel::getType() const {
    return voxelType;
}