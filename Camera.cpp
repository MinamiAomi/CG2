#include "Camera.h"

#include "Input.hpp"

void Camera::Update() {
    if (Input::IsMousePressed(MouseButton::Right)) {
        constexpr float rotSpeed = 1.0f * Math::ToRadian;
        rotate_.x += rotSpeed * Input::GetMouseMove().y * 0.1f;
        rotate_.y += rotSpeed * Input::GetMouseMove().x * 0.1f;
    }
    else if (Input::IsMousePressed(MouseButton::Mid)) {
        Matrix4x4 rotMat = Matrix4x4::MakeRotationXYZ(rotate_);
        Vector3 cameraX = rotMat.GetXAxis() * Input::GetMouseMove().x * -0.01f;
        Vector3 cameraY = rotMat.GetYAxis() * Input::GetMouseMove().y * 0.01f;
        position_ += cameraX + cameraY;
    }
    else if (Input::GetWheel()) {
        Matrix4x4 rotMat = Matrix4x4::MakeRotationXYZ(rotate_);
        Vector3 cameraZ = rotMat.GetZAxis() * (Input::GetWheel() / 120 * 0.5f);
        position_ += cameraZ;
    }

    viewMatrix_ = Matrix4x4::MakeAffineInverse(Matrix4x4::MakeRotationXYZ(rotate_), position_);
}

void Camera::SetProjectionMatrix(float fovY, float aspectRaito, float nearZ, float farZ) {
    fovY_ = fovY;
    aspectRaito_ = aspectRaito;
    nearZ_ = nearZ;
    farZ_ = farZ;
    projectionMatrix_ = Matrix4x4::MakePerspectiveProjection(fovY_, aspectRaito_, nearZ_, farZ_);
}
