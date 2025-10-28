#include "camera.h"
#include <cstring>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Camera::Camera()
    : posX(0.0f), posY(0.0f), posZ(5.0f),
      yaw(0.0f), pitch(0.0f),
      inputForward(0.0f), inputRight(0.0f), inputUp(0.0f),
      inputYaw(0.0f), inputPitch(0.0f),
      moveSpeed(3.0f), rotationSpeed(1.5f),
      fov(45.0f), nearPlane(0.1f), farPlane(100.0f) {
}

Camera::~Camera() {
}

void Camera::update(float deltaTime) {
    // Calculate position-dependent rotation modifier
    // Rotation speed varies based on camera position (height and distance from origin)
    float distanceFromOrigin = std::sqrt(posX * posX + posY * posY + posZ * posZ);
    float positionModifier = 1.0f + (distanceFromOrigin * 0.01f); // Scale factor based on distance
    
    // Update rotation with position-dependent scaling
    yaw += inputYaw * rotationSpeed * deltaTime * positionModifier;
    pitch += inputPitch * rotationSpeed * deltaTime * positionModifier;
    
    // Normalize yaw to keep it in range [-PI, PI] for consistency
    // This prevents floating point precision issues and ensures predictable behavior
    while (yaw > M_PI) yaw -= 2.0f * M_PI;
    while (yaw < -M_PI) yaw += 2.0f * M_PI;
    
    // Clamp pitch to avoid gimbal lock
    const float maxPitch = M_PI * 0.49f; // Just under 90 degrees
    if (pitch > maxPitch) pitch = maxPitch;
    if (pitch < -maxPitch) pitch = -maxPitch;
    
    // Calculate direction vectors based on current pitch and yaw
    float cosPitch = std::cos(pitch);
    float sinPitch = std::sin(pitch);
    float cosYaw = std::cos(yaw);
    float sinYaw = std::sin(yaw);
    
    // Forward direction (full 3D vector based on pitch and yaw)
    float forwardX = -sinYaw * cosPitch;
    float forwardY = sinPitch;
    float forwardZ = -cosYaw * cosPitch;
    
    // Right direction (perpendicular to forward, always horizontal)
    float rightX = cosYaw;
    float rightY = 0.0f;
    float rightZ = -sinYaw;
    
    // Up direction (perpendicular to both forward and right)
    float upX = sinYaw * sinPitch;
    float upY = cosPitch;
    float upZ = cosYaw * sinPitch;
    
    // Update position based on current view direction
    posX += (forwardX * inputForward + rightX * inputRight + upX * inputUp) * moveSpeed * deltaTime;
    posY += (forwardY * inputForward + rightY * inputRight + upY * inputUp) * moveSpeed * deltaTime;
    posZ += (forwardZ * inputForward + rightZ * inputRight + upZ * inputUp) * moveSpeed * deltaTime;
}

void Camera::setMovementInput(float forward, float right, float up) {
    inputForward = forward;
    inputRight = right;
    inputUp = up;
}

void Camera::setRotationInput(float yawInput, float pitchInput) {
    inputYaw = yawInput;
    inputPitch = pitchInput;
}

void Camera::setPosition(float x, float y, float z) {
    posX = x;
    posY = y;
    posZ = z;
}

void Camera::setRotation(float yawAngle, float pitchAngle) {
    yaw = yawAngle;
    pitch = pitchAngle;
}

void Camera::getViewMatrix(float* matrix) const {
    // Calculate camera direction
    float cosPitch = std::cos(pitch);
    float sinPitch = std::sin(pitch);
    float cosYaw = std::cos(yaw);
    float sinYaw = std::sin(yaw);
    
    // Camera forward, right, and up vectors
    float forwardX = -sinYaw * cosPitch;
    float forwardY = sinPitch;
    float forwardZ = -cosYaw * cosPitch;
    
    float rightX = cosYaw;
    float rightY = 0.0f;
    float rightZ = -sinYaw;
    
    float upX = sinYaw * sinPitch;
    float upY = cosPitch;
    float upZ = cosYaw * sinPitch;
    
    // Create view matrix (inverse of camera transform)
    // Column-major order for Vulkan/GLSL
    // Note: forward vector is negated because camera looks down -Z axis
    matrix[0] = rightX;
    matrix[1] = rightY;
    matrix[2] = rightZ;
    matrix[3] = 0.0f;
    
    matrix[4] = upX;
    matrix[5] = upY;
    matrix[6] = upZ;
    matrix[7] = 0.0f;
    
    matrix[8] = -forwardX;
    matrix[9] = -forwardY;
    matrix[10] = -forwardZ;
    matrix[11] = 0.0f;
    
    matrix[12] = -(rightX * posX + rightY * posY + rightZ * posZ);
    matrix[13] = -(upX * posX + upY * posY + upZ * posZ);
    matrix[14] = (forwardX * posX + forwardY * posY + forwardZ * posZ);
    matrix[15] = 1.0f;
}

void Camera::getProjectionMatrix(float* matrix, float aspectRatio) const {
    // Perspective projection matrix for Vulkan (right-handed, Y-down, Z in [0, 1])
    // Column-major order for Vulkan/GLSL
    float fovRad = fov * M_PI / 180.0f;
    float tanHalfFov = std::tan(fovRad / 2.0f);
    
    std::memset(matrix, 0, sizeof(float) * 16);
    
    matrix[0] = 1.0f / (aspectRatio * tanHalfFov);
    matrix[5] = -1.0f / tanHalfFov; // Negative for Vulkan's Y-down
    matrix[10] = -farPlane / (farPlane - nearPlane); // Negative for correct Z-depth mapping
    matrix[11] = -1.0f; // Negative to ensure W_clip = -Z_view (positive for visible points)
    matrix[14] = -(farPlane * nearPlane) / (farPlane - nearPlane);
}

void Camera::getMVPMatrix(float* mvp, float aspectRatio) const {
    float view[16];
    float proj[16];
    
    getViewMatrix(view);
    getProjectionMatrix(proj, aspectRatio);
    
    // MVP = Projection * View (no model matrix for now, assume identity)
    multiplyMatrices(proj, view, mvp);
}

void Camera::multiplyMatrices(const float* a, const float* b, float* result) const {
    // Matrix multiplication: result = a * b (4x4 matrices in column-major order)
    for (int col = 0; col < 4; col++) {
        for (int row = 0; row < 4; row++) {
            result[col * 4 + row] = 0.0f;
            for (int k = 0; k < 4; k++) {
                result[col * 4 + row] += a[k * 4 + row] * b[col * 4 + k];
            }
        }
    }
}

float Camera::getPosX() const { return posX; }
float Camera::getPosY() const { return posY; }
float Camera::getPosZ() const { return posZ; }
