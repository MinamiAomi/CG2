#include "GameObject.h"

#include <cassert>

#include "Component.h"

namespace CG {

    void GameObject::Initialize() {
        for (const auto& component : components_) {
            component->OnInitialize();
        }
    }

    void GameObject::Update() {
        if (isActive_) {
            UpdateWorldMatrix();
            for (const auto& component : components_) {
                if (component->IsActive()) {
                    component->OnUpdate();
                }
            }
            UpdateWorldMatrix();
        }
    }

    void GameObject::SetParent(const std::shared_ptr<GameObject>& newParent) {
        auto spParent = parent_.lock();
        // 変更する必要がない
        if (newParent == spParent) { return; }
        // 子孫を親にすることはできません。
        if (IsDescendant(newParent)) { return; }
        // 親から自分を削除
        if (spParent) {
            spParent->RemoveChild(shared_from_this());
        }
        // 親をセット
        if (newParent) {
            newParent->children_.emplace_back(shared_from_this());
        }
        parent_ = newParent;
        NotifyChildrenNeedTransformUpdating();
    }

    void GameObject::UpdateWorldMatrix() {
        auto parent = parent_.lock();
        if (parent && parent->needTransformUpdating) {
            parent->UpdateWorldMatrix();
        }
        if (needTransformUpdating) {
            worldMatrix_ = Matrix4x4::MakeAffineTransform(scale_, rotate_, translate_);
            if (parent) {
                worldMatrix_ *= parent->worldMatrix_;
            }
            worldMatrixInverse_ = worldMatrix_.Inverse();
            needTransformUpdating = false;

            for (const auto& component : components_) {
                component->OnUpdateWorldTransform();
            }
        }
    }

    bool GameObject::IsDescendant(const std::shared_ptr<GameObject>& gameObject) {
        GameObject* searchGameObject = gameObject.get();
        while (searchGameObject) {
            if (searchGameObject == this) {
                return true;
            }
            searchGameObject = searchGameObject->parent_.lock().get();
        }
        return false;
    }

    void GameObject::RemoveChild(const std::shared_ptr<GameObject>& gameObject) {
        // 元の親から自分を削除
        for (size_t i = 0; i < children_.size(); ++i) {
            // 自分を見つけた
            if (children_[i] == gameObject) {
                for (size_t j = i; j < children_.size() - 1; ++j) {
                    children_[j].swap(children_[j + 1]);
                }
                children_.pop_back();
                break;
            }
        }
    }

    void GameObject::NotifyChildrenNeedTransformUpdating() {
        if (!needTransformUpdating) {
            needTransformUpdating = true;
            for (size_t i = 0; i < children_.size(); ++i) {
                children_[i]->NotifyChildrenNeedTransformUpdating();
            }
        }
    }

}