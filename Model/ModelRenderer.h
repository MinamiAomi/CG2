#pragma once

#include <d3d12.h>
#include <wrl/client.h>

#include <memory>

#include "../Math/MathUtils.h"

class ModelInstance;

class ModelRenderer {
public:
    enum DrawPass {
        Shadow,
        Model,

        NumDrawPasses
    };

    static const uint32_t kShadowMapWidth = 1024;
    static const uint32_t kShadowMapHeight = 1024;

    static void Create(Microsoft::WRL::ComPtr<ID3D12Device> device);
    static void Destroy();
    static ModelRenderer& Get();

    void AddModelInstance(std::shared_ptr<ModelInstance> modelInstance);
    bool RemoveModelInstance(std::shared_ptr<ModelInstance> modelInstance);

    void Render(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList);

    Microsoft::WRL::ComPtr<ID3D12Resource> GetColorBuffer() const { return colorBuffer_; }

    void ResizeColorBuffer(uint32_t width, uint32_t height);

    void SetCameraMatrix(const Matrix4x4& cameraMatrix);
    void SetShadowMatrix(const Matrix4x4& shadowMatrix);

private:
    struct SceneConstant {
        Matrix4x4 cameraViewProj;
        Matrix4x4 shadowViewProj;
        Vector3 cameraWorldPos;
        float sunLightIntensity;
        Vector4 sunLightColor;
        Vector3 sunLightDirection;
    };

    void CreateRootSignature();
    void CreatePSOs();
    void CreateBuffers();
    void DrawShadowPass();
    void DrawModelPass();

    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
    Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineStateObjects_[NumDrawPasses];

    Microsoft::WRL::ComPtr<ID3D12Resource> colorBuffer_;
    Microsoft::WRL::ComPtr<ID3D12Resource> depthBuffer_;
    Microsoft::WRL::ComPtr<ID3D12Resource> shadowBuffer_;

    Microsoft::WRL::ComPtr<ID3D12Resource> sceneConstantBuffer_; // カメラ、シャドウカメラ、サンライト
    SceneConstant sceneConstant_{};
};