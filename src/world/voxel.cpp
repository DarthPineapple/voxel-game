#include "voxel.h"

Voxel::Voxel(const glm::vec3& position, VoxelType type)
    : position(position), type(type) {}

glm::vec3 Voxel::getPosition() const {
    return position;
}

VoxelType Voxel::getType() const {
    return type;
}

void Voxel::setType(VoxelType newType) {
    type = newType;
}