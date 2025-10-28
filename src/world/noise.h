#ifndef NOISE_H
#define NOISE_H

#include <vector>

class PerlinNoise {
public:
    PerlinNoise();
    PerlinNoise(unsigned int seed);
    
    // Generate 2D Perlin noise value at given coordinates
    // Returns value between -1.0 and 1.0
    float noise(float x, float y) const;
    
    // Generate 3D Perlin noise value at given coordinates
    // Returns value between -1.0 and 1.0
    float noise(float x, float y, float z) const;
    
    // Generate octave noise (multiple layers of noise for more detail)
    // octaves: number of noise layers to combine
    // persistence: amplitude multiplier for each octave (typically 0.5)
    float octaveNoise(float x, float y, int octaves, float persistence) const;

private:
    std::vector<int> permutation;
    
    float fade(float t) const;
    float lerp(float t, float a, float b) const;
    float grad(int hash, float x, float y, float z) const;
};

#endif // NOISE_H
