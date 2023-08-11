#pragma once

#include <d3d12.h>

#include <cstdint>
#include <mutex>
#include <memory>
#include <set>
#include <vector>

#include "DescriptorAllocation.h"

class DescriptorAllocatorPage;

class DescriptorAllocator {
public:
    DescriptorAllocator(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptorsPerHeap = 256);
    virtual ~DescriptorAllocator();

    // 連続したディスクリプタを割り当てる
    DescriptorAllocation Allocate(uint32_t numDescriptors = 1);
    // フレーム終了後必要のないディスクリプタを解放する
    void ReleaseStaleDescriptors(uint64_t frameNumber);

private:
    using DescriptorHeapPool = std::vector<std::shared_ptr<DescriptorAllocatorPage>>;

    std::shared_ptr<DescriptorAllocatorPage> CreateAllocatorPage();

    const D3D12_DESCRIPTOR_HEAP_TYPE heapType_;
    uint32_t numDescriptorsPerHeap_;

    DescriptorHeapPool heapPool_;
    // 利用可能なヒープのインデックス
    std::set<size_t> availableHeaps_;

    std::mutex allocationMutex_;;
};