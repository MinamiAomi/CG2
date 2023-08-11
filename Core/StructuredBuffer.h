#pragma once
#include "GPUBuffer.h"

#include "ByteAddressBuffer.h"

class StructuredBuffer : public GPUBuffer {
public:
    StructuredBuffer(const std::wstring& name = L"");
    StructuredBuffer(const D3D12_RESOURCE_DESC& resourceDesc, size_t numElements, size_t elementSize, const std::wstring& name = L"");

    virtual void CreateViews(size_t numElements, size_t elementSize) override;

    virtual D3D12_CPU_DESCRIPTOR_HANDLE GetShaderResourceView(const D3D12_SHADER_RESOURCE_VIEW_DESC* srvDesc = nullptr) const override {
        return srv_.GetDescriptorHandle();
    }
    virtual D3D12_CPU_DESCRIPTOR_HANDLE GetUnorderedAccessView(const D3D12_UNORDERED_ACCESS_VIEW_DESC* srvDesc = nullptr) const override {
        return uav_.GetDescriptorHandle();
    }

    virtual size_t GetNumElements() const { return numElements_; }
    virtual size_t GetElementSize() const { return elementSize_; }

    const ByteAddressBuffer& GetCounterBuffer() const { return counterBuffer_; }

private:
    size_t numElements_;
    size_t elementSize_;

    DescriptorAllocation srv_;
    DescriptorAllocation uav_;

    ByteAddressBuffer counterBuffer_;
};