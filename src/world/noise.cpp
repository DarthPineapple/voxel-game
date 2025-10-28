#include "noise.h"
#include <cmath>
#include <random>
#include <algorithm>

PerlinNoise::PerlinNoise() : PerlinNoise(0) {
}

PerlinNoise::PerlinNoise(unsigned int seed) {
    // Initialize permutation table
    permutation.resize(512);
    
    // Fill with values 0-255
    std::vector<int> p(256);
    for (int i = 0; i < 256; i++) {
        p[i] = i;
    }
    
    // Shuffle using seed
    std::default_random_engine engine(seed);
    std::shuffle(p.begin(), p.end(), engine);
    
    // Duplicate the permutation vector
    for (int i = 0; i < 256; i++) {
        permutation[i] = p[i];
        permutation[256 + i] = p[i];
    }
}

float PerlinNoise::fade(float t) const {
    // Fade function: 6t^5 - 15t^4 + 10t^3
    return t * t * t * (t * (t * 6 - 15) + 10);
}

float PerlinNoise::lerp(float t, float a, float b) const {
    return a + t * (b - a);
}

float PerlinNoise::grad(int hash, float x, float y, float z) const {
    // Convert lower 4 bits of hash into one of 12 gradient directions
    int h = hash & 15;
    float u = h < 8 ? x : y;
    float v = h < 4 ? y : h == 12 || h == 14 ? x : z;
    return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

float PerlinNoise::noise(float x, float y, float z) const {
    // Find unit cube that contains point
    int X = static_cast<int>(std::floor(x)) & 255;
    int Y = static_cast<int>(std::floor(y)) & 255;
    int Z = static_cast<int>(std::floor(z)) & 255;
    
    // Find relative x, y, z of point in cube
    x -= std::floor(x);
    y -= std::floor(y);
    z -= std::floor(z);
    
    // Compute fade curves for x, y, z
    float u = fade(x);
    float v = fade(y);
    float w = fade(z);
    
    // Hash coordinates of the 8 cube corners
    int A = permutation[X] + Y;
    int AA = permutation[A] + Z;
    int AB = permutation[A + 1] + Z;
    int B = permutation[X + 1] + Y;
    int BA = permutation[B] + Z;
    int BB = permutation[B + 1] + Z;
    
    // Blend results from 8 corners of cube
    float res = lerp(w, 
        lerp(v, 
            lerp(u, grad(permutation[AA], x, y, z), 
                    grad(permutation[BA], x - 1, y, z)),
            lerp(u, grad(permutation[AB], x, y - 1, z), 
                    grad(permutation[BB], x - 1, y - 1, z))),
        lerp(v, 
            lerp(u, grad(permutation[AA + 1], x, y, z - 1), 
                    grad(permutation[BA + 1], x - 1, y, z - 1)),
            lerp(u, grad(permutation[AB + 1], x, y - 1, z - 1),
                    grad(permutation[BB + 1], x - 1, y - 1, z - 1))));
    
    return res;
}

float PerlinNoise::noise(float x, float y) const {
    // 2D noise is just 3D noise with z = 0
    return noise(x, y, 0.0f);
}

float PerlinNoise::octaveNoise(float x, float y, int octaves, float persistence) const {
    float total = 0.0f;
    float frequency = 1.0f;
    float amplitude = 1.0f;
    float maxValue = 0.0f;
    
    for (int i = 0; i < octaves; i++) {
        total += noise(x * frequency, y * frequency) * amplitude;
        
        maxValue += amplitude;
        amplitude *= persistence;
        frequency *= 2.0f;
    }
    
    // Normalize to [-1, 1]
    return total / maxValue;
}
