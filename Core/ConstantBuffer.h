#pragma once
#include "GPUBuffer.h"

#include <cstdint>

#include "DescriptorAllocation.h"

class ConstantBuffer : public GPUBuffer {
public:
    ConstantBuffer(const std::wstring& name = L"");

    virtual  ~ConstantBuffer();

    virtual void CreateViews(size_t numElements, size_t elementSize) override;

    // 使用不可
    virtual D3D12_CPU_DESCRIPTOR_HANDLE GetShaderResourceView(const D3D12_SHADER_RESOURCE_VIEW_DESC* srvDesc = nullptr) const override;
    // 使用不可
    virtual D3D12_CPU_DESCRIPTOR_HANDLE GetUnorderedAccessView(const D3D12_UNORDERED_ACCESS_VIEW_DESC* uavDesc = nullptr) const override;

    D3D12_CPU_DESCRIPTOR_HANDLE GetConstantBufferView() const { return cbv_.GetDescriptorHandle(); }
    size_t GetSizeInBytes() const { return sizeInBytes_; }


private:
    size_t sizeInBytes_;
    DescriptorAllocation cbv_;
};