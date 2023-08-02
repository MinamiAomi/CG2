#pragma once

#include "DX12/DX12.h"
#include "GraphicsEngine.h"

namespace CG {

    class PostEffect {
    public:
        struct Desc {
            uint32_t width;
            uint32_t height;
            DXGI_FORMAT resourceFormat = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
            DXGI_FORMAT targetFormat = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
            float clearColor[4] = { 0.0f,0.0f,0.0f,0.0f };
            std::string pixelShaderName;
            size_t extendedDataSize = 0;
        };

        void Initialize(GraphicsEngine& graphicsEngine, const Desc& pixelShader);

        void SetRenderTarget(DX12::CommandList& commandList, const DX12::DepthStencilView& dsv);
        void Draw(DX12::CommandList& commandList);

        void TransferExtendedData(const void* src, size_t size);

    private:
        static DX12::RootSignature rootSignature_;
        static DX12::VertexBuffer vertexBuffer_;

        DX12::PipelineState pso_;

        DX12::Resource resource_;
        DX12::RenderTargetView rtv_;
        DX12::ShaderResourceView srv_;

        DX12::DynamicBuffer extendedData_;

        float clearColor_[4];
        bool isRendering;
    };

}