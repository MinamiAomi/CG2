#include "RootSignature.h"

#include <cassert>

#include "Device.h"

namespace CG::DX12 {

    RootSignatureDesc::operator const D3D12_ROOT_SIGNATURE_DESC& () {
        auto ranges = ranges_.begin();
        for (auto& itr : rootParameters_) {
            if (itr.ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE) {
                itr.DescriptorTable.NumDescriptorRanges = static_cast<uint32_t>(ranges->size());
                itr.DescriptorTable.pDescriptorRanges = ranges->data();
                ++ranges;
            }
        }

        if (!rootParameters_.empty()) {
            desc_.NumParameters = static_cast<uint32_t>(rootParameters_.size());
            desc_.pParameters = rootParameters_.data();
        }
        if (!staticSamplerDescs_.empty()) {
            desc_.NumStaticSamplers = static_cast<uint32_t>(staticSamplerDescs_.size());
            desc_.pStaticSamplers = staticSamplerDescs_.data();
        }
        desc_.Flags = flag_;

        return desc_;
    }

    void RootSignatureDesc::AddDescriptorTable(ShaderVisibility shaderVisibility) {
        ranges_.emplace_back();
        auto& rootParameter = rootParameters_.emplace_back();
        rootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
        rootParameter.DescriptorTable = {};
        rootParameter.ShaderVisibility = static_cast<D3D12_SHADER_VISIBILITY>(shaderVisibility);
    }

    void RootSignatureDesc::AddDescriptor(DescriptorType type, uint32_t shaderRegister, ShaderVisibility shaderVisibility, uint32_t registerSpace) {
        auto& rootParameter = rootParameters_.emplace_back();
        rootParameter.ParameterType = static_cast<D3D12_ROOT_PARAMETER_TYPE>(type);
        rootParameter.Descriptor.ShaderRegister = shaderRegister;
        rootParameter.Descriptor.RegisterSpace = registerSpace;
        rootParameter.ShaderVisibility = static_cast<D3D12_SHADER_VISIBILITY>(shaderVisibility);
    }

    void RootSignatureDesc::AddDescriptorRange(RangeType rangeType, uint32_t baseShaderRegister, uint32_t numDescriptors, uint32_t registerSpace, uint32_t offset) {
        assert(numDescriptors > 0);
        assert(!ranges_.empty());

        auto& descriptorRange = ranges_.back().emplace_back();
        descriptorRange.RangeType = static_cast<D3D12_DESCRIPTOR_RANGE_TYPE>(rangeType);
        descriptorRange.NumDescriptors = numDescriptors;
        descriptorRange.BaseShaderRegister = baseShaderRegister;
        descriptorRange.RegisterSpace = registerSpace;
        descriptorRange.OffsetInDescriptorsFromTableStart = offset;
    }

    void RootSignatureDesc::AddStaticSampler(
        uint32_t shaderRegister,
        ShaderVisibility shaderVisibility,
        uint32_t ragisterSpace, 
        AddressMode addressU, 
        AddressMode addressV, 
        AddressMode addressW, 
        D3D12_FILTER filter) {
        auto& staticSamplerDesc = staticSamplerDescs_.emplace_back();
        staticSamplerDesc.Filter = filter;
        staticSamplerDesc.AddressU = static_cast<D3D12_TEXTURE_ADDRESS_MODE>(addressU);
        staticSamplerDesc.AddressV = static_cast<D3D12_TEXTURE_ADDRESS_MODE>(addressV);
        staticSamplerDesc.AddressW = static_cast<D3D12_TEXTURE_ADDRESS_MODE>(addressW);
        staticSamplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
        staticSamplerDesc.MinLOD = 0.0f;
        staticSamplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
        staticSamplerDesc.ShaderRegister = shaderRegister;
        staticSamplerDesc.RegisterSpace = ragisterSpace;
        staticSamplerDesc.ShaderVisibility = static_cast<D3D12_SHADER_VISIBILITY>(shaderVisibility);
    }

    void RootSignatureDesc::AddFlag(D3D12_ROOT_SIGNATURE_FLAGS flag) {
        flag_ |= flag;
    }

    void RootSignatureDesc::Clear() {
        desc_ = {};
        ranges_.clear();
        rootParameters_.clear();
        staticSamplerDescs_.clear();
        flag_ = D3D12_ROOT_SIGNATURE_FLAG_NONE;
    }

    void RootSignature::Initialize(const Device& device, const D3D12_ROOT_SIGNATURE_DESC& desc) {
        assert(device.IsEnabled());

        rootSignature_.Reset();

        ComPtr<ID3DBlob> blob;
        ComPtr<ID3DBlob> errorBlob;

        if (FAILED(D3D12SerializeRootSignature(
            &desc,
            D3D_ROOT_SIGNATURE_VERSION_1,
            blob.GetAddressOf(),
            errorBlob.GetAddressOf()))) {
            OutputDebugStringA(static_cast<char*>(errorBlob->GetBufferPointer()));
            assert(false);
        }

        if (FAILED(device.GetDevice()->CreateRootSignature(
            0,
            blob->GetBufferPointer(),
            blob->GetBufferSize(),
            IID_PPV_ARGS(rootSignature_.GetAddressOf())))) {
            assert(false);
        }

    }

}