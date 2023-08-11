#include "RootSignature.h"

#include "../Externals/DirectXTex/d3dx12.h"

#include "Defines.h"
#include "Graphics.h"

RootSignature::RootSignature() :
    rootSignatureDesc_{},
    numDescriptorsPerTable_{},
    samplerTableBitMask_(0),
    descriptorTableBitMask_(0) {
}

RootSignature::RootSignature(const D3D12_ROOT_SIGNATURE_DESC1& rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION rootSignatureVersion) :
    rootSignatureDesc_{},
    numDescriptorsPerTable_{},
    samplerTableBitMask_(0),
    descriptorTableBitMask_(0) {
    Create(rootSignatureDesc, rootSignatureVersion);
}

RootSignature::~RootSignature() {
    Destroy();
}

void RootSignature::Create(const D3D12_ROOT_SIGNATURE_DESC1& rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION rootSignatureVersion) {
    Destroy();

    auto device = Graphics::Get().GetDevice();

    uint32_t numParameters = rootSignatureDesc.NumParameters;
    D3D12_ROOT_PARAMETER1* pParameters = numParameters > 0 ? new D3D12_ROOT_PARAMETER1[numParameters] : nullptr;

    for (uint32_t i = 0; i < numParameters; ++i) {
        const auto& rootParameter = rootSignatureDesc.pParameters[i];
        pParameters[i] = rootParameter;

        if (rootParameter.ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE) {
            uint32_t numDescriptorRanges = rootParameter.DescriptorTable.NumDescriptorRanges;
            D3D12_DESCRIPTOR_RANGE1* pDescriptorRanges = numDescriptorRanges > 0 ? new D3D12_DESCRIPTOR_RANGE1[numDescriptorRanges] : nullptr;

            memcpy(pDescriptorRanges, rootParameter.DescriptorTable.pDescriptorRanges, sizeof(D3D12_DESCRIPTOR_RANGE1) * numDescriptorRanges);

            pParameters[i].DescriptorTable.NumDescriptorRanges = numDescriptorRanges;
            pParameters[i].DescriptorTable.pDescriptorRanges = pDescriptorRanges;
            
            // ビットマスクに設定
            if (numDescriptorRanges > 0) {
                switch (pDescriptorRanges[0].RangeType) {
                case D3D12_DESCRIPTOR_RANGE_TYPE_CBV:
                case D3D12_DESCRIPTOR_RANGE_TYPE_SRV:
                case D3D12_DESCRIPTOR_RANGE_TYPE_UAV:
                    descriptorTableBitMask_ |= (1 << i);
                    break;
                case D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER:
                    samplerTableBitMask_ |= (1 << i);
                    break;
                default:
                }
            }
            // テーブル内のディスクリプタの数を数える
            for (uint32_t j = 0; j < numDescriptorRanges; ++j) {
                numDescriptorsPerTable_[i] += pDescriptorRanges[j].NumDescriptors;
            }
        }
    }

    rootSignatureDesc_.NumParameters = numParameters;
    rootSignatureDesc_.pParameters = pParameters;

    uint32_t numStaticSamplers = rootSignatureDesc_.NumStaticSamplers;
    D3D12_STATIC_SAMPLER_DESC* pStaticSamplers = numStaticSamplers > 0 ? new D3D12_STATIC_SAMPLER_DESC[numStaticSamplers] : nullptr;

    if (pStaticSamplers) {
        memcpy(pStaticSamplers, rootSignatureDesc.pStaticSamplers, sizeof(D3D12_STATIC_SAMPLER_DESC) * numStaticSamplers);
    }
    rootSignatureDesc_.NumStaticSamplers = numStaticSamplers;
    rootSignatureDesc_.pStaticSamplers = pStaticSamplers;

    D3D12_ROOT_SIGNATURE_FLAGS flags = rootSignatureDesc.Flags;
    rootSignatureDesc_.Flags = flags;

    CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC versionRootSignatureDesc{};
    versionRootSignatureDesc.Init_1_1(numParameters, pParameters, numStaticSamplers, pStaticSamplers, flags);

    Microsoft::WRL::ComPtr<ID3DBlob> rootSignatureBlob;
    Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;
    ASSERT_IF_FAILED(D3DX12SerializeVersionedRootSignature(&versionRootSignatureDesc, rootSignatureVersion, rootSignatureBlob.GetAddressOf(), errorBlob.GetAddressOf()));

    ASSERT_IF_FAILED(device->CreateRootSignature(0, rootSignatureBlob->GetBufferPointer(), rootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(rootSignature_.GetAddressOf())));

    NAME_D3D12_OBJECT(rootSignature_);
}

void RootSignature::Destroy() {
    for (uint32_t i = 0; i < rootSignatureDesc_.NumParameters; ++i) {
        const D3D12_ROOT_PARAMETER1& rootParameter = rootSignatureDesc_.pParameters[i];
        if (rootParameter.ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE) {
            delete[] rootParameter.DescriptorTable.pDescriptorRanges;
        }
    }

    delete[] rootSignatureDesc_.pParameters;
    rootSignatureDesc_.pParameters = nullptr;
    rootSignatureDesc_.NumParameters = 0;

    delete[] rootSignatureDesc_.pStaticSamplers;
    rootSignatureDesc_.pStaticSamplers = nullptr;
    rootSignatureDesc_.NumStaticSamplers = 0;

    descriptorTableBitMask_ = 0;
    samplerTableBitMask_ = 0;

    memset(numDescriptorsPerTable_, 0, sizeof(numDescriptorsPerTable_));
}

uint32_t RootSignature::GetDescriptorTableBitMask(D3D12_DESCRIPTOR_HEAP_TYPE descriptorHeapType) const {
    uint32_t descriptorTableBitMask = 0;
    switch (descriptorHeapType) {
    case D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV:
        descriptorTableBitMask = descriptorTableBitMask_;
        break;
    case D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER:
        descriptorTableBitMask = samplerTableBitMask_;
        break;
    default:
    }

    return descriptorTableBitMask;;
}

uint32_t RootSignature::GetNumDescriptors(uint32_t rootIndex) const {
    ASSERT(rootIndex < kMaxDescriptorTables);
    return numDescriptorsPerTable_[rootIndex];
}
