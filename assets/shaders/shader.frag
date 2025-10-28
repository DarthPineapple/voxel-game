#version 450

// Input from vertex shader
layout(location = 0) in vec3 fragNormal;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in float fragWorldY;

// Output color
layout(location = 0) out vec4 fragColor;

void main() {
    // Simple lighting based on normal direction
    vec3 lightDir = normalize(vec3(0.5, 1.0, 0.3));
    float diffuse = max(dot(normalize(fragNormal), lightDir), 0.2);
    
    // Color variation based on elevation (world Y coordinate)
    // Lower elevations (Y < 0) are darker, higher elevations (Y > 32) are lighter
    float elevationFactor = clamp(fragWorldY / 64.0, 0.0, 1.0);
    
    // Create a gradient from darker to lighter based on elevation
    // Dark blue-green at low elevations, transitioning to lighter colors at high elevations
    vec3 lowColor = vec3(0.5, 0.6, 0.7);   // Darker shade for low elevations
    vec3 highColor = vec3(0.9, 1.0, 1.1);  // Lighter shade for high elevations
    
    vec3 baseColor = mix(lowColor, highColor, elevationFactor);
    fragColor = vec4(baseColor * diffuse, 1.0);
}