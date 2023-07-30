#pragma once

#include <cstdint>

namespace CG {

    class  GameObject;

    class Component {
    public:
        Component(GameObject& gameObject) : gameObject(gameObject) {}
        virtual ~Component() = 0 {}

        virtual void OnInitialize() {}
        virtual void OnUpdate() {}
        virtual void OnUpdateWorldTransform() {}
        virtual void OnPreRender() {}
        virtual void OnRenderObject() {}
        virtual void OnInspectorView() {}

        void SetIsActive(bool isActive) { isActive_ = isActive; }
        bool IsActive() const { return isActive_; }

        GameObject& gameObject;

    private:
        bool isActive_ = true;
    };

}