#pragma once

#include <d3d12.h>
#include <wrl/client.h>

#include <cstdint>
#include <vector>

namespace CG::DX12 {
    using namespace Microsoft::WRL;

    class Device;

    enum class ShaderVisibility {
        All = D3D12_SHADER_VISIBILITY_ALL,
        Vertex = D3D12_SHADER_VISIBILITY_VERTEX,
        Hull = D3D12_SHADER_VISIBILITY_HULL,
        Domain = D3D12_SHADER_VISIBILITY_DOMAIN,
        Geometry = D3D12_SHADER_VISIBILITY_GEOMETRY,
        Pixel = D3D12_SHADER_VISIBILITY_PIXEL,
        Amplification = D3D12_SHADER_VISIBILITY_AMPLIFICATION,
        Mesh = D3D12_SHADER_VISIBILITY_MESH
    };

    enum class DescriptorType {
        CBV = D3D12_ROOT_PARAMETER_TYPE_CBV,
        SRV = D3D12_ROOT_PARAMETER_TYPE_SRV,
        UAV = D3D12_ROOT_PARAMETER_TYPE_UAV
    };

    enum class RangeType {
        SRV = D3D12_DESCRIPTOR_RANGE_TYPE_SRV,
        UAV = D3D12_DESCRIPTOR_RANGE_TYPE_UAV,
        CBV = D3D12_DESCRIPTOR_RANGE_TYPE_CBV,
        Sampler = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER
    };

    enum class AddressMode {
        Wrap = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
        Mirror = D3D12_TEXTURE_ADDRESS_MODE_MIRROR,
        Clamp = D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
        Border = D3D12_TEXTURE_ADDRESS_MODE_BORDER,
        MirrorOnce = D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE
    };

    class RootSignatureDesc {
    public:
        operator const D3D12_ROOT_SIGNATURE_DESC& ();

        void AddDescriptorTable(ShaderVisibility shaderVisibility);
        void AddDescriptor(DescriptorType descType, uint32_t shaderRegister, ShaderVisibility shaderVisibility, uint32_t registerSpace = 0);
        void AddDescriptorRange(
            RangeType rangeType,
            uint32_t baseShaderRegister,
            uint32_t numDescriptors,
            uint32_t registerSpace = 0,
            uint32_t offset = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND);
        void AddStaticSampler(
            uint32_t shaderRegister,
            ShaderVisibility shaderVisibility,
            uint32_t ragisterSpace = 0,
            AddressMode addressU = AddressMode::Wrap,
            AddressMode addressV = AddressMode::Wrap,
            AddressMode addressW = AddressMode::Wrap,
            D3D12_FILTER filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR);
        void AddFlag(D3D12_ROOT_SIGNATURE_FLAGS flag);
        void Clear();

    private:
        std::vector<std::vector<D3D12_DESCRIPTOR_RANGE>> ranges_;
        std::vector<D3D12_ROOT_PARAMETER> rootParameters_;
        std::vector<D3D12_STATIC_SAMPLER_DESC> staticSamplerDescs_;
        D3D12_ROOT_SIGNATURE_FLAGS flag_ = D3D12_ROOT_SIGNATURE_FLAG_NONE;
        D3D12_ROOT_SIGNATURE_DESC desc_{};

    };

    class RootSignature {
    public:
        void Initialize(const Device& device, const D3D12_ROOT_SIGNATURE_DESC& desc);

        bool IsEnabled() const { return rootSignature_; }
        ComPtr<ID3D12RootSignature> GetRootSignature() const { return rootSignature_; }

    private:
        ComPtr<ID3D12RootSignature> rootSignature_;
    };

}