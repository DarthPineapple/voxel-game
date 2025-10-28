#version 450

// Vertex input attributes (must match Vertex struct in mesh.h)
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

// Uniform buffer for MVP matrix
layout(binding = 0) uniform UniformBufferObject {
    mat4 mvp;
} ubo;

// Output to fragment shader
layout(location = 0) out vec3 fragNormal;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out float fragWorldY;

void main() {
    gl_Position = ubo.mvp * vec4(inPosition, 1.0);
    fragNormal = inNormal;
    fragTexCoord = inTexCoord;
    fragWorldY = inPosition.y;
}