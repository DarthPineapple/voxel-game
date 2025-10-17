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
    // Update rotation
    yaw += inputYaw * rotationSpeed * deltaTime;
    pitch += inputPitch * rotationSpeed * deltaTime;
    
    // Clamp pitch to avoid gimbal lock
    const float maxPitch = M_PI * 0.49f; // Just under 90 degrees
    if (pitch > maxPitch) pitch = maxPitch;
    if (pitch < -maxPitch) pitch = -maxPitch;
    
    // Calculate forward and right vectors based on yaw
    float cosYaw = std::cos(yaw);
    float sinYaw = std::sin(yaw);
    
    // Forward direction (in XZ plane)
    float forwardX = -sinYaw;
    float forwardZ = -cosYaw;
    
    // Right direction (perpendicular to forward in XZ plane)
    float rightX = cosYaw;
    float rightZ = -sinYaw;
    
    // Update position
    posX += (forwardX * inputForward + rightX * inputRight) * moveSpeed * deltaTime;
    posZ += (forwardZ * inputForward + rightZ * inputRight) * moveSpeed * deltaTime;
    posY += inputUp * moveSpeed * deltaTime;
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
    
    float upX = -sinYaw * sinPitch;
    float upY = cosPitch;
    float upZ = -cosYaw * sinPitch;
    
    // Create view matrix (inverse of camera transform)
    // Row-major order for Vulkan
    matrix[0] = rightX;
    matrix[1] = upX;
    matrix[2] = forwardX;
    matrix[3] = 0.0f;
    
    matrix[4] = rightY;
    matrix[5] = upY;
    matrix[6] = forwardY;
    matrix[7] = 0.0f;
    
    matrix[8] = rightZ;
    matrix[9] = upZ;
    matrix[10] = forwardZ;
    matrix[11] = 0.0f;
    
    matrix[12] = -(rightX * posX + rightY * posY + rightZ * posZ);
    matrix[13] = -(upX * posX + upY * posY + upZ * posZ);
    matrix[14] = -(forwardX * posX + forwardY * posY + forwardZ * posZ);
    matrix[15] = 1.0f;
}

void Camera::getProjectionMatrix(float* matrix, float aspectRatio) const {
    // Perspective projection matrix for Vulkan (right-handed, Y-down, Z in [0, 1])
    float fovRad = fov * M_PI / 180.0f;
    float tanHalfFov = std::tan(fovRad / 2.0f);
    
    std::memset(matrix, 0, sizeof(float) * 16);
    
    matrix[0] = 1.0f / (aspectRatio * tanHalfFov);
    matrix[5] = -1.0f / tanHalfFov; // Negative for Vulkan's Y-down
    matrix[10] = farPlane / (farPlane - nearPlane);
    matrix[11] = 1.0f;
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
    // Matrix multiplication: result = a * b (4x4 matrices)
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            result[i * 4 + j] = 0.0f;
            for (int k = 0; k < 4; k++) {
                result[i * 4 + j] += a[i * 4 + k] * b[k * 4 + j];
            }
        }
    }
}
