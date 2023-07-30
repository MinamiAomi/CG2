#pragma once

#include "DX12/DX12.h"
#include "Math/MathUtils.h"

namespace CG {

    class GraphicsEngine;
    class TextureManager;

    class Material {
    public:
        enum RootParameter {
            kTransformation,
            kMaterial,
            kLight,
            kTexture,

            kRootParamterCount
        };
        enum Lighting {
            kNoneLighting,
            kLambertLighting,
            kHalfLambertLighting,

            kLightingCount
        };

        static void StaticInitialize(GraphicsEngine* graphicsEngine, TextureManager* textureManager);
        static void DrawSetting(DX12::CommandList& commandList, D3D12_GPU_VIRTUAL_ADDRESS lightBuffer);
        static GraphicsEngine* graphicsEngine_;
        static TextureManager* textureManager_;
        static DX12::RootSignature rootSignature_;
        static DX12::PipelineState pipelineStates_[kLightingCount];

        void Initialize();
        void TransferData();
        void SetGraphicsRoot(DX12::CommandList& commandList);

        void Edit(const std::string& label);

        void SetColor(const Vector4& color) { color_ = color; }
        void SetUVOffset(const Vector2& uvOffset) { uvOffset_ = uvOffset; }
        void SetUVRotate(float uvRotate) { uvRotate_ = uvRotate; }
        void SetUVScale(const Vector2& uvScale) { uvScale_ = uvScale; }
        void SetTextureHandle(uint32_t textureHandle) { textureHandle_ = textureHandle; }
        void SetUseTexture(bool useTexture) { useTexture_ = useTexture; }
        void SetLighting(Lighting lighting) { lighting_ = lighting; }
    private:
        DX12::DynamicBuffer constantBuffer_;
        Vector4 color_ = Vector4::one;
        Vector2 uvOffset_;
        float uvRotate_;
        Vector2 uvScale_ = Vector2::one;
        uint32_t textureHandle_ = 0;
        Lighting lighting_ = kHalfLambertLighting;
        bool useTexture_ = true;
    };

}