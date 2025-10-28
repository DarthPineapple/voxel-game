#version 450

// Vertex input for simple 2D position
layout(location = 0) in vec2 inPosition;

// Push constants for rotation angle
layout(push_constant) uniform PushConstants {
    float angle;
} pc;

void main() {
    // Apply rotation around center
    float cosA = cos(pc.angle);
    float sinA = sin(pc.angle);
    
    vec2 rotated;
    rotated.x = inPosition.x * cosA - inPosition.y * sinA;
    rotated.y = inPosition.x * sinA + inPosition.y * cosA;
    
    // Position at top-left of screen in NDC space
    // Scale down and offset to top-left corner
    vec2 position = rotated * 0.1 - vec2(0.85, 0.85);
    
    gl_Position = vec4(position, 0.0, 1.0);
}
