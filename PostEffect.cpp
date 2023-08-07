#include "PostEffect.h"

#include <cassert>

#include "Math/MathUtils.h"

namespace CG {

    DX12::RootSignature PostEffect::rootSignature_;
    DX12::VertexBuffer PostEffect::vertexBuffer_;


    void PostEffect::Initialize(GraphicsEngine& graphicsEngine, const Desc& desc) {
        if (!rootSignature_.IsEnabled()) {
            DX12::RootSignatureDesc rootSignatureDesc;
            rootSignatureDesc.AddDescriptorTable(DX12::ShaderVisibility::Pixel);
            rootSignatureDesc.AddDescriptorRange(DX12::RangeType::SRV, 0, 1);
            rootSignatureDesc.AddDescriptor(DX12::DescriptorType::CBV, 0, DX12::ShaderVisibility::Pixel);
            rootSignatureDesc.AddStaticSampler(0, DX12::ShaderVisibility::Pixel, 0, DX12::AddressMode::Clamp, DX12::AddressMode::Clamp, DX12::AddressMode::Clamp);
            rootSignatureDesc.AddFlag(D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
            rootSignature_.Initialize(graphicsEngine.GetDevice(), rootSignatureDesc);
        }
        if (!vertexBuffer_.resource.IsEnabled()) {
            constexpr size_t kVertexCount = 6;
            constexpr size_t kVertexStrideSize = sizeof(Vector3) + sizeof(Vector2);
            constexpr size_t kVertexBufferSize = kVertexStrideSize * kVertexCount;
            vertexBuffer_.resource.InitializeForBuffer(graphicsEngine.GetDevice(), kVertexBufferSize);
            vertexBuffer_.view.Initialize(vertexBuffer_.resource, kVertexBufferSize, kVertexStrideSize, kVertexCount);

            float vertices[kVertexCount * 5] = {
                -1.0f,1.0f,0.0f, 0.0f,0.0f, // 左上
                1.0f,1.0f,0.0f, 1.0f,0.0f, // 右上
                1.0f,-1.0f,0.0f, 1.0f,1.0f, // 右下
                -1.0f,1.0f,0.0f, 0.0f,0.0f, // 左上
                1.0f,-1.0f,0.0f, 1.0f,1.0f, // 右下
                -1.0f,-1.0f,0.0f, 0.0f,1.0f, // 左下
            };
            auto dataBegin = vertexBuffer_.resource.Map();
            memcpy(dataBegin, vertices, sizeof(vertices));
            vertexBuffer_.resource.Unmap();
        }

        auto pixelShader = graphicsEngine.GetShaderCompiler().Compile(desc.pixelShaderName, L"ps_6_0");
        auto vertexShader = graphicsEngine.GetShaderCompiler().Compile("PostEffect.VS.hlsl", L"vs_6_0");

        DX12::GraphicsPipelineStateDesc psoDesc;
        psoDesc.SetRootSignature(rootSignature_);
        psoDesc.SetVertexShader(vertexShader);
        psoDesc.SetPixelShader(pixelShader);
        psoDesc.AddInputElementVertex("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0);
        psoDesc.AddInputElementVertex("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0);
        psoDesc.SetSampleState();
        psoDesc.AddRenderTargetState(DX12::BlendMode::None, desc.targetFormat);
        psoDesc.SetDepthState(DXGI_FORMAT_D24_UNORM_S8_UINT, DX12::ComparisonFunc::LessEqual, false);
        psoDesc.SetPrimitiveTopologyType(DX12::PrimitiveTopology::Triangle);
        psoDesc.SetRasterizerState(DX12::CullMode::None);
        pso_.Initialize(graphicsEngine.GetDevice(), psoDesc);

        resource_.InitializeForTexture2D(
            graphicsEngine.GetDevice(),
            desc.width, desc.height, 1, 1,
            desc.resourceFormat, DX12::Resource::State::Common,
            DX12::Resource::HeapType::Default, DX12::Resource::Flag::AllowRenderTarget, desc.clearColor);

        rtv_.Initialize(graphicsEngine.GetDevice(), resource_, graphicsEngine.GetRTVDescriptorHeap().Allocate());
        srv_.InitializeTexture2D(graphicsEngine.GetDevice(), resource_, graphicsEngine.GetSRVDescriptorHeap().Allocate());

        if (desc.extendedDataSize > 0) {
            extendedData_.Initialize(graphicsEngine.GetDevice(), desc.extendedDataSize);
        }

        memcpy(clearColor_, desc.clearColor, sizeof(clearColor_));

        viewport_ = DX12::Viewport(float(desc.width), float(desc.height));
        scissorRect_ = DX12::ScissorRect(viewport_);
        isRendering = false;

        viewport_ = DX12::Viewport(float(desc.width), float(desc.height));
        scissorRect_ = DX12::ScissorRect(viewport_);
    }

    void PostEffect::SetRenderTarget(DX12::CommandList& commandList, const DX12::DepthStencilView& dsv) {
        auto cmdList = commandList.GetCommandList().Get();

        auto barrier = resource_.TransitionBarrier(DX12::Resource::State::RenderTarget);
        cmdList->ResourceBarrier(1, &barrier);

        auto rtv = rtv_.GetDescriptor().GetCPUHandle();
        auto dsvHandle = dsv.GetDescriptor().GetCPUHandle();
        cmdList->OMSetRenderTargets(1, &rtv, false, &dsvHandle);
        cmdList->ClearRenderTargetView(rtv, clearColor_, 0, nullptr);
        cmdList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

        cmdList->RSSetViewports(1, &viewport_);
        cmdList->RSSetScissorRects(1, &scissorRect_);

        isRendering = true;
    }

    void PostEffect::Draw(DX12::CommandList& commandList) {
        auto cmdList = commandList.GetCommandList().Get();

        isRendering = false;

        auto barrier = resource_.TransitionBarrier(DX12::Resource::State::PixelShaderResource);
        cmdList->ResourceBarrier(1, &barrier);

        cmdList->SetGraphicsRootSignature(rootSignature_.GetRootSignature().Get());
        cmdList->SetPipelineState(pso_.GetPipelineState().Get());
        cmdList->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        cmdList->IASetVertexBuffers(0, 1, &vertexBuffer_.view.GetView());
        cmdList->SetGraphicsRootDescriptorTable(0, srv_.GetDescriptor().GetGPUHandle());
        if (extendedData_.GetResource().IsEnabled()) {
            cmdList->SetGraphicsRootConstantBufferView(1, extendedData_.GetResource().GetGPUVirtualAddress());
        }
        cmdList->DrawInstanced(vertexBuffer_.view.GetVertexCount(), 1, 0, 0);
    }

    void PostEffect::TransferExtendedData(const void* src, size_t size) {
        assert(0 < size && size <= extendedData_.GetBufferSize());
        if (extendedData_.GetDataBegin() != nullptr && src) {
            memcpy(extendedData_.GetDataBegin(), src, size);
        }
    }

}