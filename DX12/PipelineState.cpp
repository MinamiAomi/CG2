#include "PipelineState.h"

#include <cassert>

#include "Device.h"
#include "RootSignature.h"
#include "Shader.h"

namespace CG::DX12 {

    GraphicsPipelineStateDesc::operator const D3D12_GRAPHICS_PIPELINE_STATE_DESC& () {
        desc_.NumRenderTargets = renderTargetCount_;
        auto elem = inputElements_.begin();
        auto name = semanticNames_.begin();
        for (; elem != inputElements_.end(); ++elem, ++name) {
            elem->SemanticName = name->c_str();
        }
        desc_.InputLayout.pInputElementDescs = inputElements_.data();
        desc_.InputLayout.NumElements = static_cast<uint32_t>(inputElements_.size());
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

    void GraphicsPipelineStateDesc::AddInputElementVertex(const std::string& semanticName, uint32_t semanticIndex, DXGI_FORMAT format, uint32_t inputSlot) {
        auto& inputElement = inputElements_.emplace_back();
        semanticNames_.push_back(semanticName);
        inputElement.SemanticIndex = semanticIndex;
        inputElement.Format = format;
        inputElement.InputSlot = inputSlot;
        inputElement.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
        inputElement.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
        inputElement.InstanceDataStepRate = 0;
    }

    void GraphicsPipelineStateDesc::AddInputElementInstance(const std::string& semanticName, uint32_t semanticIndex, DXGI_FORMAT format, uint32_t inputSlot, uint32_t instanceDataStepRate) {
        auto& inputElement = inputElements_.emplace_back();
        semanticNames_.push_back(semanticName);
        inputElement.SemanticIndex = semanticIndex;
        inputElement.Format = format;
        inputElement.InputSlot = inputSlot;
        inputElement.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
        inputElement.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA;
        inputElement.InstanceDataStepRate = instanceDataStepRate;
    }

    void GraphicsPipelineStateDesc::SetRasterizerState(CullMode cullMode, FillMode fillMode) {
        desc_.RasterizerState.FillMode = static_cast<D3D12_FILL_MODE>(fillMode);
        desc_.RasterizerState.CullMode = static_cast<D3D12_CULL_MODE>(cullMode);
    }

    void GraphicsPipelineStateDesc::SetSampleState(uint32_t count, uint32_t quality) {
        desc_.SampleDesc.Count = count;
        desc_.SampleDesc.Quality = quality;
    }

    void GraphicsPipelineStateDesc::SetPrimitiveTopologyType(PrimitiveTopology primitiveTopology) {
        desc_.PrimitiveTopologyType = static_cast<D3D12_PRIMITIVE_TOPOLOGY_TYPE>(primitiveTopology);
    }

    void GraphicsPipelineStateDesc::AddRenderTargetState(BlendMode blendMode, DXGI_FORMAT rtvFormat) {
        assert(renderTargetCount_ < 8);
        auto& renderTarget = desc_.BlendState.RenderTarget[renderTargetCount_];
        desc_.RTVFormats[renderTargetCount_] = rtvFormat;
        ++renderTargetCount_;

        renderTarget.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
        renderTarget.BlendOpAlpha = D3D12_BLEND_OP_ADD;
        renderTarget.SrcBlendAlpha = D3D12_BLEND_ONE;
        renderTarget.DestBlendAlpha = D3D12_BLEND_ZERO;
        renderTarget.BlendEnable = true;


        switch (blendMode) {
        case BlendMode::None:
            renderTarget.BlendEnable = false;
            return;
        case BlendMode::Normal:
        default:
            renderTarget.BlendOp = D3D12_BLEND_OP_ADD;
            renderTarget.SrcBlend = D3D12_BLEND_SRC_ALPHA;
            renderTarget.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;	// 1.0f-ソースのアルファ値
            return;
        case BlendMode::Add:
            renderTarget.BlendOp = D3D12_BLEND_OP_ADD;				// 加算
            renderTarget.SrcBlend = D3D12_BLEND_SRC_ALPHA;				// ソースの値を 100% 使う
            renderTarget.DestBlend = D3D12_BLEND_ONE;				// デストの値を 100% 使う
            return;
        case BlendMode::Subtract:
            renderTarget.BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;	// デストからソースを減算
            renderTarget.SrcBlend = D3D12_BLEND_SRC_ALPHA;				// ソースの値を 100% 使う
            renderTarget.DestBlend = D3D12_BLEND_ONE;				// デストの値を 100% 使う
            return;
        case BlendMode::Multiply:
            renderTarget.BlendOp = D3D12_BLEND_OP_ADD;				// 加算
            renderTarget.SrcBlend = D3D12_BLEND_ZERO;				// 使わない
            renderTarget.DestBlend = D3D12_BLEND_SRC_COLOR;		// デストの値 × ソースの値
            return;
        case BlendMode::Inverse:
            renderTarget.BlendOp = D3D12_BLEND_OP_ADD;				// 加算
            renderTarget.SrcBlend = D3D12_BLEND_INV_DEST_COLOR;	// 1.0f-デストカラーの値
            renderTarget.DestBlend = D3D12_BLEND_ZERO;				// 使わない
            return;
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

    void GraphicsPipelineStateDesc::Clear() {
        desc_ = {};
        renderTargetCount_ = 0;
        inputElements_.clear();
        semanticNames_.clear();
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
