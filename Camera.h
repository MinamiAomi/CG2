#pragma once
#include "Component.h"

#include <memory>

#include "Math/MathUtils.h"

namespace CG {

    class Camera : public Component {
    public:
        enum class Projection {
            Perspective,
            Orthographic
        };

        static const std::shared_ptr<Camera>& GetMainCamera() { return mainCamera_; }
        static void SetMainCamera(const std::shared_ptr<Camera>& camera) { mainCamera_ = camera; }

        Camera(GameObject& gameObject);

        virtual void OnInitialize() override;
        virtual void OnUpdateWorldTransform() override;
        virtual void OnPreRender() override;
        virtual void OnInspectorView() override {}

        void SetProjection(Projection projection) { projection_ = projection; needMatrixUpdating_ = true; }
        void SetFovY(float fovY) { perspective_.fovY  = fovY;needMatrixUpdating_ = true; }
        void SetAspectRaito(float aspectRaito) { perspective_.aspectRaito  = aspectRaito; needMatrixUpdating_ = true;}
        void SetWidth(float width) { orthographic_.width  = width; needMatrixUpdating_ = true;}
        void SetHeight(float height) { orthographic_.height  = height; needMatrixUpdating_ = true;}
        void SetNearZ(float nearZ) { nearZ_ = nearZ; needMatrixUpdating_ = true;}
        void SetFarZ(float farZ) { farZ_ = farZ; needMatrixUpdating_ = true;}
        
        const Matrix4x4& GetViewMatrix() { return viewMatrix_; }
        const Matrix4x4& GetViewMatrixInverse() { return viewMatrixInverse_; }
        const Matrix4x4& GetProjectionMatrix() { return projectionMatrix_; }
        const Matrix4x4& GetProjectionMatrixInverse() { return projectionMatrixInverse_; }
        const Matrix4x4& GetViewProjectionMatrix() { return viewProjectionMatrix_; }
        const Matrix4x4& GetViewProjectionMatrixInverse() { return viewProjectionMatrixInverse_; }

    private:
        static std::shared_ptr<Camera> mainCamera_;
        static size_t count_;

        struct Perspective {
            float fovY, aspectRaito;
        };
        struct Orthographic {
            float width, height;
        };

        Projection projection_;
        Perspective perspective_;
        Orthographic orthographic_;
        float nearZ_;
        float farZ_;

        Matrix4x4 viewMatrix_;
        Matrix4x4 viewMatrixInverse_;
        Matrix4x4 projectionMatrix_;
        Matrix4x4 projectionMatrixInverse_;
        Matrix4x4 viewProjectionMatrix_;
        Matrix4x4 viewProjectionMatrixInverse_;

        bool needMatrixUpdating_;
    };

}