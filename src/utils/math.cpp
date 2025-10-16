#include "math.h"
#include <cmath>

// Function to calculate the dot product of two 3D vectors
float dot(const Vector3& a, const Vector3& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

// Function to calculate the cross product of two 3D vectors
Vector3 cross(const Vector3& a, const Vector3& b) {
    return Vector3{
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}

// Function to normalize a 3D vector
Vector3 normalize(const Vector3& v) {
    float length = std::sqrt(dot(v, v));
    if (length > 0) {
        return Vector3{ v.x / length, v.y / length, v.z / length };
    }
    return Vector3{ 0, 0, 0 }; // Return a zero vector if the length is zero
}

// Function to calculate the length of a 3D vector
float length(const Vector3& v) {
    return std::sqrt(dot(v, v));
}