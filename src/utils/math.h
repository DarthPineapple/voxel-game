#ifndef MATH_H
#define MATH_H

#include <cmath>

namespace Math {

    inline float lerp(float a, float b, float t) {
        return a + t * (b - a);
    }

    inline float clamp(float value, float min, float max) {
        return std::fmax(min, std::fmin(value, max));
    }

    inline float toRadians(float degrees) {
        return degrees * (M_PI / 180.0f);
    }

    inline float toDegrees(float radians) {
        return radians * (180.0f / M_PI);
    }

    struct Vec3 {
        float x, y, z;

        Vec3 operator+(const Vec3& other) const {
            return { x + other.x, y + other.y, z + other.z };
        }

        Vec3 operator-(const Vec3& other) const {
            return { x - other.x, y - other.y, z - other.z };
        }

        Vec3 operator*(float scalar) const {
            return { x * scalar, y * scalar, z * scalar };
        }

        float length() const {
            return std::sqrt(x * x + y * y + z * z);
        }

        Vec3 normalize() const {
            float len = length();
            return { x / len, y / len, z / len };
        }
    };

}

#endif // MATH_H