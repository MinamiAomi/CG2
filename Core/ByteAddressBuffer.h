#pragma once
#include "GPUBuffer.h"

#include "DescriptorAllocation.h"

class ByteAddressBuffer : public GPUBuffer {
public:
    ByteAddressBuffer(const std::wstring& name = L"");
    ByteAddressBuffer(const D3D12_RESOURCE_DESC& resourceDesc, size_t numElements, size_t elementSize, const std::wstring& name = L"");

    virtual void CreateViews(size_t numElements, size_t elementSize) override;

    virtual D3D12_CPU_DESCRIPTOR_HANDLE GetShaderResourceView(const D3D12_SHADER_RESOURCE_VIEW_DESC* srvDesc = nullptr) const override {
        return srv_.GetDescriptorHandle();
    }
    virtual D3D12_CPU_DESCRIPTOR_HANDLE GetUnorderedAccessView(const D3D12_UNORDERED_ACCESS_VIEW_DESC* srvDesc = nullptr) const override {
        return uav_.GetDescriptorHandle();
    }

    size_t GetBufferSize() const { return bufferSize_; }

private:
    size_t bufferSize_;

    DescriptorAllocation srv_;
    DescriptorAllocation uav_;
};