#pragma once
#include "Component.h"

#include "DX12/DX12.h"
#include "Math/MathUtils.h"
#include "ResourceManager.h"

namespace CG {

    class Model;

    class ModelRenderer : public Component {
    public:
        using Component::Component;

        void OnInitialize() override {}
        void OnUpdateWorldTransform() override {}
        void OnRenderObject() override {}
        void OnInspectorView() override {}

        void SetModel(std::shared_ptr<Model> model) { model_ = model; }
        void SetColor(const Vector4& color) { color_ = color; }
        void SetUVOffset(const Vector2& uvOffset) { uvOffset_ = uvOffset; }
        void SetUVRotate(float uvRotate) { uvRotate_ = uvRotate; }
        void SetUVScale(const Vector2& uvScale) { uvOffset_ = uvScale; }
        void SetUseTexture(bool useTexture) { useTexture_ = useTexture; }
        void SetIsLighting(bool isLighting) { isLighting_ = isLighting; }

    private:
        std::shared_ptr<Model> model_;
        DX12::DynamicBuffer transformationBuffer_;
        std::vector<DX12::DynamicBuffer> materialBuffer_;
        std::vector<std::shared_ptr<TextureSet>> texture_;
        Vector4 color_;
        Vector2 uvOffset_;
        float uvRotate_;
        Vector2 uvScale_;
        bool useTexture_;
        bool isLighting_;
    };

}
