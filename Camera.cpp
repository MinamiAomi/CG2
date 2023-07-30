#include "Camera.h"

#include "GameObject.h"

namespace CG {

    std::shared_ptr<Camera> Camera::mainCamera_;
    size_t Camera::count_ = 0;

    Camera::Camera(GameObject& gameObject) : Component(gameObject) {
        ++count_;
    }

    void Camera::OnInitialize() {
        projection_ = Projection::Perspective;
        perspective_.fovY = 45.0f * Math::ToRadian;
        nearZ_ = 0.1f;
        farZ_ = 1000.0f;
    }

    void Camera::OnUpdateWorldTransform() {
        Matrix4x4 cameraMatrix = gameObject.GetWorldMatrix();
        viewMatrix_ = gameObject.GetWorldMatrixInverse();
        viewMatrixInverse_ = gameObject.GetWorldMatrix();

        viewProjectionMatrix_ = viewMatrix_ * projectionMatrix_;
        viewProjectionMatrixInverse_ = viewMatrixInverse_ * projectionMatrixInverse_;
    }

    void Camera::OnPreRender() {
        if (needMatrixUpdating_) {
            switch (projection_) {
            case CG::Camera::Projection::Perspective:
                projectionMatrix_ = Matrix4x4::MakePerspectiveProjection(perspective_.fovY, perspective_.aspectRaito, nearZ_, farZ_);
                break;
            case CG::Camera::Projection::Orthographic:
                projectionMatrix_ = Matrix4x4::MakeOrthographicProjection(orthographic_.width, orthographic_.height, nearZ_, farZ_);
                break;
            }
            projectionMatrixInverse_ = projectionMatrix_.Inverse();
            viewProjectionMatrix_ = viewMatrix_ * projectionMatrix_;
            viewProjectionMatrixInverse_ = viewMatrixInverse_ * projectionMatrixInverse_;
            needMatrixUpdating_ = false;
        }
    }

}