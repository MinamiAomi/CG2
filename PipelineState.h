#pragma once

#include <d3d12.h>
#include <wrl/client.h>

#include <vector>
#include <string>

namespace CG::DX12 {
    using namespace Microsoft::WRL;

    class Device;
    class RootSignature;
    class Shader;

    enum class BlendMode {
        None,		// 合成なし
        Normal,		// 通常
        Add,		// 加算合成
        Subtract,	// 減算合成
        Multiply,	// 乗算合成
        Inverse		// 反転
    };
    // カリングモード
    enum class CullMode {
        None = D3D12_CULL_MODE_NONE,	// カリングしない
        Front = D3D12_CULL_MODE_FRONT,	// 前面カリング
        Back = D3D12_CULL_MODE_BACK,	// 背面カリング
    };
    // 比較関数
    enum class ComparisonFunc {
        Never = D3D12_COMPARISON_FUNC_NEVER,				// 比較に成功しない
        Less = D3D12_COMPARISON_FUNC_LESS,					// 小さい場合
        Equal = D3D12_COMPARISON_FUNC_EQUAL,				// 等しい場合
        LessEqual = D3D12_COMPARISON_FUNC_LESS_EQUAL,		// 以下の場合
        Greater = D3D12_COMPARISON_FUNC_GREATER,			// より大きい場合
        NotEqual = D3D12_COMPARISON_FUNC_NOT_EQUAL,			// 等しくない場合
        GreaterEqual = D3D12_COMPARISON_FUNC_GREATER_EQUAL,	// 以上の場合
        Always = D3D12_COMPARISON_FUNC_ALWAYS				// 比較を渡す
    };
    // プリミティブ型
    enum class PrimitiveTopology {
        Point = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT,		// 点
        Line = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE,			// 線
        Triangle = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE	// 三角形
    };

    class BlendDesc {
        friend class GraphicsPipelineStateDesc;
    public:
        void AddRenderTargetDesc(BlendMode blendMode, DXGI_FORMAT format);
    private:
        D3D12_BLEND_DESC desc_{};
        DXGI_FORMAT rtvFormats_[8];
        uint32_t renderTargetCount_{ 0 };
    };

    class InputLayoutDesc {
        friend class GraphicsPipelineStateDesc;
    public:
        void AddVertex(const std::string& semanticName, uint32_t semanticIndex, DXGI_FORMAT format, uint32_t inputSlot);
        void AddInstance(const std::string& semanticName, uint32_t semanticIndex, DXGI_FORMAT format, uint32_t inputSlot, uint32_t instanceDataStepRate);
    private:
        D3D12_INPUT_LAYOUT_DESC desc_;
        std::vector<D3D12_INPUT_ELEMENT_DESC> elements_;
        std::vector<std::string> semanticNames_;
    };

    class GraphicsPipelineStateDesc {
    public:
        operator const D3D12_GRAPHICS_PIPELINE_STATE_DESC& ();

        void SetRootSignature(const RootSignature& rootSignature);
        void SetVertexShader(const Shader& shader);
        void SetPixelShader(const Shader& shader);
        void SetDomeinShader(const Shader& shader);
        void SetHullShader(const Shader& shader);
        void SetGeometryShader(const Shader& shader);
        void SetInputLayout(const InputLayoutDesc& inputLayout);
        void SetRasterizerState(CullMode cullMode, bool fillModeSolidOrWireFrame = true);
        void SetSampleState(uint32_t count, uint32_t quality);
        void SetPrimitiveTopologyType(PrimitiveTopology primitiveTopology);
        void SetBlendDesc(const BlendDesc& blendDesc);
        void SetDepthState(DXGI_FORMAT dsvFormat, ComparisonFunc comparisonFunc = ComparisonFunc::LessEqual, bool depthWriteMaskAllOrZero = true);
        void SetStencilState(uint8_t readMask, uint8_t writeMask);
    private:
        D3D12_GRAPHICS_PIPELINE_STATE_DESC desc_;
    };

    class ComputePipelineStateDesc {
    public:
        operator const D3D12_COMPUTE_PIPELINE_STATE_DESC& ();

        void SetRootSignature(const RootSignature& rootSignature);
        void SetComputeShader(const Shader& shader);
    private:
        D3D12_COMPUTE_PIPELINE_STATE_DESC desc_;
    };

    class PipelineState {
    public:
        void Initialize(const Device& device, const D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc);
        void Initialize(const Device& device, const D3D12_COMPUTE_PIPELINE_STATE_DESC& desc);

        bool IsEnabled() const { return pipelineState_; }
        ComPtr<ID3D12PipelineState> GetPipelineState() const { return pipelineState_; }
    private:
        ComPtr<ID3D12PipelineState> pipelineState_;
    };

   

}
