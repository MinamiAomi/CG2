#pragma once

#include <d3d12.h>

#include <cstdint>
#include <memory>

class DescriptorAllocatorPage;

class DescriptorAllocation{
public:
    DescriptorAllocation();
    DescriptorAllocation(D3D12_CPU_DESCRIPTOR_HANDLE descriptor, uint32_t numHandles, uint32_t descriptorSize, std::shared_ptr<DescriptorAllocatorPage> page);
    // 自動的に解放する
    ~DescriptorAllocation();
    // コピー禁止
    DescriptorAllocation(const DescriptorAllocation& copy) = delete;
    DescriptorAllocation& operator=(const DescriptorAllocation& copy) = delete;
    // ムーブは許可
    DescriptorAllocation(DescriptorAllocation&& move) noexcept;
    DescriptorAllocation& operator=(DescriptorAllocation&& move) noexcept;

    D3D12_CPU_DESCRIPTOR_HANDLE GetDescriptorHandle(uint32_t offset = 0) const;
    uint32_t GetNumHandles() const { return numHandles_; }
    std::shared_ptr<DescriptorAllocatorPage> GetDescriptorAllocatorPage() const { return page_; }

    bool IsNull() const { return descriptor_.ptr == 0; }

private:
    void Free();

    D3D12_CPU_DESCRIPTOR_HANDLE descriptor_;
    uint32_t numHandles_;
    uint32_t descriptorSize_;
    // 割り当て元
    std::shared_ptr<DescriptorAllocatorPage> page_;    
};