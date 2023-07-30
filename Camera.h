#pragma once

#include "Math/MathUtils.h"

class Camera {
public:
    void Update();

    void SetProjectionMatrix(float fovY, float aspectRaito, float nearZ, float farZ);

    const Matrix4x4& GetViewMatrix() const { return viewMatrix_; }
    const Matrix4x4& GetProjectionMatrix() const { return projectionMatrix_; }

private:
    Vector3 position_ = { 0.0f,1.0f,-6.0f };
    Vector3 rotate_ = { 0,0,0 };
    Matrix4x4 viewMatrix_;

    float fovY_;
    float aspectRaito_;
    float nearZ_;
    float farZ_;
    Matrix4x4 projectionMatrix_;
};