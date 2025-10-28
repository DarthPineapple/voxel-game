# Elevation-Based Shading

## Overview

The voxel game now features elevation-based shading that creates visual depth by applying different colors to blocks at different heights. Lower elevations appear darker, while higher elevations appear lighter, creating a natural gradient effect across the terrain.

## Implementation

### Shader Changes

The elevation-based shading is implemented in the GLSL shaders:

#### Vertex Shader (`assets/shaders/shader.vert`)

The vertex shader now passes the world Y coordinate to the fragment shader:

```glsl
layout(location = 2) out float fragWorldY;

void main() {
    gl_Position = ubo.mvp * vec4(inPosition, 1.0);
    fragNormal = inNormal;
    fragTexCoord = inTexCoord;
    fragWorldY = inPosition.y;  // Pass world Y coordinate to fragment shader
}
```

#### Fragment Shader (`assets/shaders/shader.frag`)

The fragment shader uses the world Y coordinate to calculate an elevation factor and blend between two colors:

```glsl
layout(location = 2) in float fragWorldY;

void main() {
    // Simple lighting based on normal direction
    vec3 lightDir = normalize(vec3(0.5, 1.0, 0.3));
    float diffuse = max(dot(normalize(fragNormal), lightDir), 0.2);
    
    // Color variation based on elevation (world Y coordinate)
    // Lower elevations (Y < 0) are darker, higher elevations (Y > 32) are lighter
    float elevationFactor = clamp(fragWorldY / 64.0, 0.0, 1.0);
    
    // Create a gradient from darker to lighter based on elevation
    vec3 lowColor = vec3(0.5, 0.6, 0.7);   // Darker shade for low elevations
    vec3 highColor = vec3(0.9, 1.0, 1.1);  // Lighter shade for high elevations
    
    vec3 baseColor = mix(lowColor, highColor, elevationFactor);
    fragColor = vec4(baseColor * diffuse, 1.0);
}
```

## How It Works

### Elevation Factor Calculation

The elevation factor is calculated by normalizing the world Y coordinate:

```glsl
float elevationFactor = clamp(fragWorldY / 64.0, 0.0, 1.0);
```

- Blocks at Y = 0 have an elevation factor of 0.0 (darkest)
- Blocks at Y = 64 have an elevation factor of 1.0 (lightest)
- The `clamp` function ensures values stay in the [0.0, 1.0] range

### Color Gradient

Two colors are defined to create the gradient:

- **Low Color**: `vec3(0.5, 0.6, 0.7)` - A darker blue-gray for low elevations
- **High Color**: `vec3(0.9, 1.0, 1.1)` - A lighter white-blue for high elevations

The `mix` function blends between these colors based on the elevation factor:

```glsl
vec3 baseColor = mix(lowColor, highColor, elevationFactor);
```

### Lighting Integration

The elevation-based color is then multiplied by the diffuse lighting factor, which is calculated based on the surface normal direction:

```glsl
fragColor = vec4(baseColor * diffuse, 1.0);
```

This ensures that the elevation shading works together with the directional lighting system.

## Customization

### Adjusting the Elevation Range

To change the height range over which the color gradient is applied, modify the divisor in the elevation factor calculation:

```glsl
// For a narrower gradient (0-32 blocks):
float elevationFactor = clamp(fragWorldY / 32.0, 0.0, 1.0);

// For a wider gradient (0-128 blocks):
float elevationFactor = clamp(fragWorldY / 128.0, 0.0, 1.0);
```

### Changing Colors

To modify the appearance, adjust the low and high color values:

```glsl
// More contrast (darker low, brighter high):
vec3 lowColor = vec3(0.3, 0.4, 0.5);
vec3 highColor = vec3(1.0, 1.1, 1.2);

// Warmer tones:
vec3 lowColor = vec3(0.5, 0.5, 0.4);   // Brownish
vec3 highColor = vec3(1.0, 0.95, 0.9); // Warm white

// Cooler tones:
vec3 lowColor = vec3(0.4, 0.5, 0.6);   // Cool blue
vec3 highColor = vec3(0.8, 0.9, 1.0);  // Light blue
```

### Non-Linear Gradients

For more dramatic effects, you can apply power functions to the elevation factor:

```glsl
// Darker at low elevations, rapidly brightening:
float elevationFactor = pow(clamp(fragWorldY / 64.0, 0.0, 1.0), 2.0);

// Slowly brightening, with rapid change at high elevations:
float elevationFactor = pow(clamp(fragWorldY / 64.0, 0.0, 1.0), 0.5);
```

## Shader Compilation

After making changes to the shaders, you must recompile them before running the application:

```bash
./compile_shaders.sh
```

This will compile the shaders to SPIR-V format, which is required by Vulkan.

## Visual Effects

The elevation-based shading creates several visual benefits:

1. **Depth Perception**: The gradient helps players judge the relative height of terrain features
2. **Visual Variety**: Different elevations have distinct appearances, making the terrain more interesting
3. **Natural Look**: Mimics atmospheric effects where higher elevations receive more light
4. **Terrain Recognition**: Players can quickly identify valleys, hills, and mountains by color

## Performance Impact

Elevation-based shading has minimal performance impact:

- Adds only one additional varying value passed from vertex to fragment shader
- Fragment shader calculations are simple (one division, one clamp, one mix)
- No texture lookups or complex calculations required
- Performance cost is negligible compared to overall rendering

## Future Enhancements

Potential improvements to the elevation-based shading system:

- [ ] Add biome-specific color palettes (desert, forest, snow, etc.)
- [ ] Implement smooth transitions between biomes
- [ ] Add time-of-day lighting that affects elevation colors
- [ ] Include atmospheric fog effects for distant high terrain
- [ ] Add seasonal color variations
