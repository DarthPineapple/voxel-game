#ifndef TERRAIN_CONFIG_H
#define TERRAIN_CONFIG_H

// Terrain generation configuration
namespace TerrainConfig {
    // Noise seed for consistent terrain generation
    constexpr unsigned int NOISE_SEED = 12345;
    
    // Terrain generation parameters
    constexpr float SCALE = 0.05f;              // Controls terrain frequency
    constexpr int OCTAVES = 4;                  // Number of noise layers
    constexpr float PERSISTENCE = 0.5f;         // Amplitude decay for each octave
    constexpr float HEIGHT_MULTIPLIER = 20.0f;  // Maximum terrain height variation
    constexpr int BASE_HEIGHT = 32;             // Base terrain level
}

#endif // TERRAIN_CONFIG_H
