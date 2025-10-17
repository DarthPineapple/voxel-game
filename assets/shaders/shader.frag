#version 450

// Input from vertex shader
layout(location = 0) in vec3 fragNormal;
layout(location = 1) in vec2 fragTexCoord;

// Output color
layout(location = 0) out vec4 fragColor;

void main() {
    // Simple lighting based on normal direction
    vec3 lightDir = normalize(vec3(0.5, 1.0, 0.3));
    float diffuse = max(dot(normalize(fragNormal), lightDir), 0.2);
    
    // Color based on texture coordinates for visual variety
    vec3 baseColor = vec3(0.7, 0.8, 0.9);
    fragColor = vec4(baseColor * diffuse, 1.0);
}