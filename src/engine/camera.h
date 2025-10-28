#ifndef CAMERA_H
#define CAMERA_H

#include <cmath>

class Camera {
public:
    Camera();
    ~Camera();

    void update(float deltaTime);
    
    // Movement control
    void setMovementInput(float forward, float right, float up);
    void setRotationInput(float pitch, float yaw);
    
    // Get view and projection matrices
    void getViewMatrix(float* matrix) const;
    void getProjectionMatrix(float* matrix, float aspectRatio) const;
    void getMVPMatrix(float* mvp, float aspectRatio) const;
    
    // Camera parameters
    void setPosition(float x, float y, float z);
    void setRotation(float yaw, float pitch);
    void setMoveSpeed(float speed) { moveSpeed = speed; }
    void setRotationSpeed(float speed) { rotationSpeed = speed; }
    
    float getPositionX() const { return posX; }
    float getPositionY() const { return posY; }
    float getPositionZ() const { return posZ; }
    float getPosX() const;
    float getPosY() const;
    float getPosZ() const;
    float getYaw() const { return yaw; }
    float getPitch() const { return pitch; }

private:
    // Camera position
    float posX, posY, posZ;
    
    // Camera rotation (in radians)
    float yaw, pitch;
    
    // Input values
    float inputForward, inputRight, inputUp;
    float inputYaw, inputPitch;
    
    // Movement parameters
    float moveSpeed;
    float rotationSpeed;
    float fov;
    float nearPlane;
    float farPlane;
    
    void multiplyMatrices(const float* a, const float* b, float* result) const;
};

#endif // CAMERA_H
