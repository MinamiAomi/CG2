#pragma once

#include <d3d12.h>
#include <wrl/client.h>

#include <cstdint>
#include <map>
#include <memory>
#include <mutex>
#include <queue>

#include "DescriptorAllocation.h"

class DescriptorAllocatorPage : public std::enable_shared_from_this<DescriptorAllocatorPage> {
public:
    DescriptorAllocatorPage(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptors);

    // 連続したディスクリプタを割り当てる
    // 失敗した場合Nullになる
    DescriptorAllocation Allocate(uint32_t numDescriptors);
    // ディスクリプタの解放する
    // ReleaseStaleDescriptorsにて
    // 解放フレームが終了したら完全に解放される
    void Free(DescriptorAllocation&& descriptor, uint64_t frameNumber);
    // 不必要になったディスクリプタを解放する
    void ReleaseStaleDescriptors(uint64_t frameNumber);

    D3D12_DESCRIPTOR_HEAP_TYPE GetHeapType() const { return heapType_; }
    uint32_t GetNumFreeHandles() const { return numFreeHandles_; }

    bool HasSpace(uint32_t numDescriptors) const;

private:
    using OffsetType = uint32_t;
    using SizeType = uint32_t;

    struct FreeBlockInfo;
    // フリーリストをオフセットとサイズの双方向マップで管理
    // オフセット順のマップ
    using FreeListByOffset = std::map<OffsetType, FreeBlockInfo>;
    // サイズ順のマップ
    // 同じサイズの可能性があるのでmultimap
    using FreeListBySize = std::multimap<SizeType, FreeListByOffset::iterator>;

    struct FreeBlockInfo {
        FreeBlockInfo(SizeType size) : size(size) {}
        SizeType size;
        FreeListBySize::iterator freeListBySizeIter;
    };
    // 解放待ちディスクリプタ
    struct StaleDescriptorInfo {
        StaleDescriptorInfo(OffsetType offset, SizeType size, uint64_t frame) :
            offset(offset), size(size), frameNumber(frame) {}
        OffsetType offset;
        SizeType size;
        uint64_t frameNumber;
    };
    using StaleDescriptorQueue = std::queue<StaleDescriptorInfo>;

    uint32_t ComputeOffset(D3D12_CPU_DESCRIPTOR_HANDLE handle);
    // 新しいブロックをフリーリストに追加する
    void AddNewBlock(uint32_t offset, uint32_t numDescriptors);
    // ブロックを解放する
    // 空きブロックをマージしてより大きなブロックにする。
    void FreeBlock(uint32_t offset, uint32_t numDescriptors);

    const D3D12_DESCRIPTOR_HEAP_TYPE heapType_;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap_;
    D3D12_CPU_DESCRIPTOR_HANDLE baseDescriptor_;
    uint32_t descriptorSize_;
    uint32_t numDescriptors_;
    uint32_t numFreeHandles_;

    FreeListByOffset freeListByOffset_;
    FreeListBySize freeListBySize_;
    StaleDescriptorQueue staleDescriptors_;

    std::mutex allocationMutex_;
};