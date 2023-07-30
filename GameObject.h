#pragma once

#include <cstdint>
#include <memory>
#include <vector>
#include <typeinfo>

#include "Math/MathUtils.h"

namespace CG {

    class Component;

    class GameObject :
        public std::enable_shared_from_this<GameObject> {
    public:
        void Initialize();
        void Update();

        void SetTranslate(const Vector3& translate) { translate_ = translate, NotifyChildrenNeedTransformUpdating(); }
        const Vector3& GetTranslate() const { return translate_; }
        void SetRotate(const Vector3& rotate) { rotate_ = rotate, NotifyChildrenNeedTransformUpdating(); }
        const Vector3& GetRotate() const { return rotate_; }
        void SetScale(const Vector3& scale) { scale_ = scale, NotifyChildrenNeedTransformUpdating(); }
        const Vector3& GetScale() const { return scale_; }

        const Matrix4x4& GetWorldMatrix() const { return worldMatrix_; }
        const Matrix4x4& GetWorldMatrixInverse() const { return worldMatrixInverse_; }
        Vector3 GetWorldPosition() const { return worldMatrix_.GetTranslate(); }

        void SetParent(const std::shared_ptr<GameObject>& newParent);
        const std::weak_ptr<GameObject>& GetParent() const { return parent_; }
        const std::vector<std::shared_ptr<GameObject>>& GetChildren() const { return children_; }

        const std::vector<std::shared_ptr<Component>>& GetComponents() const { return components_; }

        template<class T>
        std::shared_ptr<T> AddComponent() {
            static_assert(std::is_base_of<Component, T>::value, "Not inherit Component");
            auto component = GetComponent<T>();
            if (component) { return component; }
            component = std::make_shared<T>(*this);
            components_.emplace_back(component);
            return component;
        }
        template<class T>
        std::shared_ptr<T> GetComponent() {
            for (const auto& c : components_) {
                if (typeid(T) == typeid(*c)) { return std::static_pointer_cast<T>(c); }
            }
            return nullptr;
        }
        template<class T>
        void RemoveComponent() {
            auto iter = std::find_if(components_.begin(), components_.end(),
                [](const auto& component) {
                    return typeid(T) == typeid(*component); });
            components_.erase(iter);
        }

        void SetIsActive(bool isActive) { isActive_ = isActive; }
        bool IsActive() const { return isActive_; }

    private:
        void UpdateWorldMatrix();
        bool IsDescendant(const std::shared_ptr<GameObject>& gameObject);
        void RemoveChild(const std::shared_ptr<GameObject>& gameObject);
        void NotifyChildrenNeedTransformUpdating();

        Vector3 translate_{ 0.0f,0.0f,0.0f };
        Vector3 rotate_{ 0.0f,0.0f,0.0f };
        Vector3 scale_{ 1.0f,1.0f,1.0f };
        Matrix4x4 worldMatrix_;
        Matrix4x4 worldMatrixInverse_;

        std::weak_ptr<GameObject> parent_;
        std::vector<std::shared_ptr<GameObject>> children_;

        std::vector<std::shared_ptr<Component>> components_;

        bool isActive_ = true;
        bool needTransformUpdating = false;
    };

}