#include "PipelineState.h"

#include <cassert>

#include "Device.h"
#include "RootSignature.h"
#include "Shader.h"

namespace CG::DX12 {

    void BlendDesc::AddRenderTargetDesc(BlendMode blendMode, DXGI_FORMAT format) {
        assert(renderTargetCount_ < 8);
        auto& rt = desc_.RenderTarget[renderTargetCount_];
        rtvFormats_[renderTargetCount_] = format;
        ++renderTargetCount_;

        rt.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
        rt.BlendOpAlpha = D3D12_BLEND_OP_ADD;			// 加算
        rt.SrcBlendAlpha = D3D12_BLEND_ONE;			// ソースの値を 100% 使う
        rt.DestBlendAlpha = D3D12_BLEND_ZERO;			// デストの値を   0% 使う
        rt.BlendEnable = true;						// ブレンドを有効にする

        switch (blendMode)
        {
        case BlendMode::None:
            rt.BlendEnable = false;
            return;
        case BlendMode::Normal:
        default:
            rt.BlendOp = D3D12_BLEND_OP_ADD;
            rt.SrcBlend = D3D12_BLEND_SRC_ALPHA;
            rt.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;	// 1.0f-ソースのアルファ値
            return;
        case BlendMode::Add:
            rt.BlendOp = D3D12_BLEND_OP_ADD;				// 加算
            rt.SrcBlend = D3D12_BLEND_SRC_ALPHA;				// ソースの値を 100% 使う
            rt.DestBlend = D3D12_BLEND_ONE;				// デストの値を 100% 使う
            return;
        case BlendMode::Subtract:
            rt.BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;	// デストからソースを減算
            rt.SrcBlend = D3D12_BLEND_SRC_ALPHA;				// ソースの値を 100% 使う
            rt.DestBlend = D3D12_BLEND_ONE;				// デストの値を 100% 使う
            return;
        case BlendMode::Multiply:
            rt.BlendOp = D3D12_BLEND_OP_ADD;				// 加算
            rt.SrcBlend = D3D12_BLEND_ZERO;				// 使わない
            rt.DestBlend = D3D12_BLEND_SRC_COLOR;		// デストの値 × ソースの値
            return;
        case BlendMode::Inverse:
            rt.BlendOp = D3D12_BLEND_OP_ADD;				// 加算
            rt.SrcBlend = D3D12_BLEND_INV_DEST_COLOR;	// 1.0f-デストカラーの値
            rt.DestBlend = D3D12_BLEND_ZERO;				// 使わない
            return;
        }
    }

    void InputLayoutDesc::AddVertex(const std::string& semanticName, uint32_t semanticIndex, DXGI_FORMAT format, uint32_t inputSlot) {
        auto& element = elements_.emplace_back();
        semanticNames_.push_back(semanticName);
        element.SemanticName = semanticNames_.back().c_str();
        element.SemanticIndex = semanticIndex;
        element.Format = format;
        element.InputSlot = inputSlot;
        element.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
        element.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
        element.InstanceDataStepRate = 0;
    }

    void InputLayoutDesc::AddInstance(const std::string& semanticName, uint32_t semanticIndex, DXGI_FORMAT format, uint32_t inputSlot, uint32_t instanceDataStepRate) {
        auto& element = elements_.emplace_back();
        semanticNames_.push_back(semanticName);
        element.SemanticName = semanticNames_.back().c_str();
        element.SemanticIndex = semanticIndex;
        element.Format = format;
        element.InputSlot = inputSlot;
        element.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
        element.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA;
        element.InstanceDataStepRate = instanceDataStepRate;
    }

    GraphicsPipelineStateDesc::operator const D3D12_GRAPHICS_PIPELINE_STATE_DESC& () {
        desc_.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
        return desc_;
    }

    void GraphicsPipelineStateDesc::SetRootSignature(const RootSignature& rootSignature) {
        desc_.pRootSignature = rootSignature.GetRootSignature().Get();
    }

    void GraphicsPipelineStateDesc::SetVertexShader(const Shader& shader) {
        desc_.VS.pShaderBytecode = shader.GetPointer();
        desc_.VS.BytecodeLength = shader.GetSize();
    }

    void GraphicsPipelineStateDesc::SetPixelShader(const Shader& shader) {
        desc_.PS.pShaderBytecode = shader.GetPointer();
        desc_.PS.BytecodeLength = shader.GetSize();
    }

    void GraphicsPipelineStateDesc::SetDomeinShader(const Shader& shader) {
        desc_.DS.pShaderBytecode = shader.GetPointer();
        desc_.DS.BytecodeLength = shader.GetSize();
    }

    void GraphicsPipelineStateDesc::SetHullShader(const Shader& shader) {
        desc_.HS.pShaderBytecode = shader.GetPointer();
        desc_.HS.BytecodeLength = shader.GetSize();
    }

    void GraphicsPipelineStateDesc::SetGeometryShader(const Shader& shader) {
        desc_.GS.pShaderBytecode = shader.GetPointer();
        desc_.GS.BytecodeLength = shader.GetSize();
    }

    void GraphicsPipelineStateDesc::SetInputLayout(const InputLayoutDesc& inputLayout) {
        desc_.InputLayout.pInputElementDescs = inputLayout.elements_.data();
        desc_.InputLayout.NumElements = static_cast<uint32_t>(inputLayout.elements_.size());
    }

    void GraphicsPipelineStateDesc::SetRasterizerState(CullMode cullMode, bool fillModeSolidOrWireFrame) {
        desc_.RasterizerState.FillMode = fillModeSolidOrWireFrame ? D3D12_FILL_MODE_SOLID : D3D12_FILL_MODE_WIREFRAME;
        desc_.RasterizerState.CullMode = static_cast<D3D12_CULL_MODE>(cullMode);
    }

    void GraphicsPipelineStateDesc::SetSampleState(uint32_t count, uint32_t quality) {
        desc_.SampleDesc.Count = count;
        desc_.SampleDesc.Quality = quality;
    }

    void GraphicsPipelineStateDesc::SetPrimitiveTopologyType(PrimitiveTopology primitiveTopology) {
        desc_.PrimitiveTopologyType = static_cast<D3D12_PRIMITIVE_TOPOLOGY_TYPE>(primitiveTopology);
    }

    void GraphicsPipelineStateDesc::SetBlendDesc(const BlendDesc& blendDesc) {
        desc_.BlendState.AlphaToCoverageEnable = blendDesc.desc_.AlphaToCoverageEnable;
        desc_.BlendState.IndependentBlendEnable = blendDesc.desc_.IndependentBlendEnable;
        desc_.NumRenderTargets = blendDesc.renderTargetCount_;
        for (uint32_t i = 0; i < blendDesc.renderTargetCount_; ++i) {
            desc_.BlendState.RenderTarget[i] = blendDesc.desc_.RenderTarget[i];
            desc_.RTVFormats[i] = blendDesc.rtvFormats_[i];
        }
    }

    void GraphicsPipelineStateDesc::SetDepthState(DXGI_FORMAT dsvFormat, ComparisonFunc comparisonFunc, bool depthWriteMaskAllOrZero) {
        desc_.DepthStencilState.DepthEnable = true;
        desc_.DepthStencilState.DepthFunc = static_cast<D3D12_COMPARISON_FUNC>(comparisonFunc);
        desc_.DepthStencilState.DepthWriteMask = depthWriteMaskAllOrZero ? D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK_ZERO;
        desc_.DSVFormat = dsvFormat;
    }

    void GraphicsPipelineStateDesc::SetStencilState(uint8_t readMask, uint8_t writeMask) {
        desc_.DepthStencilState.StencilEnable = true;
        desc_.DepthStencilState.StencilReadMask = readMask;
        desc_.DepthStencilState.StencilWriteMask = writeMask;
    }

    ComputePipelineStateDesc::operator const D3D12_COMPUTE_PIPELINE_STATE_DESC& () {
        return desc_;
    }

    void ComputePipelineStateDesc::SetRootSignature(const RootSignature& rootSignature) {
        desc_.pRootSignature = rootSignature.GetRootSignature().Get();
    }

    void ComputePipelineStateDesc::SetComputeShader(const Shader& shader) {
        desc_.CS.pShaderBytecode = shader.GetPointer();
        desc_.CS.BytecodeLength = shader.GetSize();
    }

    void PipelineState::Initialize(const Device& device, const D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc) {
        assert(device.IsEnabled());
        pipelineState_.Reset();
        if (FAILED(device.GetDevice()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(pipelineState_.GetAddressOf())))) {
            assert(false);
        }
    }

    void PipelineState::Initialize(const Device& device, const D3D12_COMPUTE_PIPELINE_STATE_DESC& desc) {
        assert(device.IsEnabled());
        pipelineState_.Reset();
        if (FAILED(device.GetDevice()->CreateComputePipelineState(&desc, IID_PPV_ARGS(pipelineState_.GetAddressOf())))) {
            assert(false);
        }
    }

}
