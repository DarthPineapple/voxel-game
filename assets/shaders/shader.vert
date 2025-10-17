#version 450

// Vertex input attributes (must match Vertex struct in mesh.h)
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

// Output to fragment shader
layout(location = 0) out vec3 fragNormal;
layout(location = 1) out vec2 fragTexCoord;

void main() {
    // Simple orthographic-style projection for now
    // Scale down and apply basic transformation
    vec3 pos = inPosition * 0.05 - vec3(0.4, 0.4, 0.0);
    gl_Position = vec4(pos.x, pos.y, 0.0, 1.0);
    
    fragNormal = inNormal;
    fragTexCoord = inTexCoord;
}