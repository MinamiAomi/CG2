#include "DescriptorAllocatorPage.h"

#include "Defines.h"
#include "Graphics.h"

DescriptorAllocatorPage::DescriptorAllocatorPage(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptors) :
    heapType_(type),
    numDescriptors_(numDescriptors) {
    auto device = Graphics::Get().GetDevice();

    D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
    descriptorHeapDesc.Type = heapType_;
    descriptorHeapDesc.NumDescriptors = numDescriptors_;

    ASSERT_IF_FAILED(device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(descriptorHeap_.GetAddressOf())));

    NAME_D3D12_OBJECT(descriptorHeap_);

    baseDescriptor_ = descriptorHeap_->GetCPUDescriptorHandleForHeapStart();
    descriptorSize_ = device->GetDescriptorHandleIncrementSize(heapType_);
    numFreeHandles_ = numDescriptors_;

    AddNewBlock(0, numFreeHandles_);
}

DescriptorAllocation DescriptorAllocatorPage::Allocate(uint32_t numDescriptors) {
    std::lock_guard<std::mutex> lock(allocationMutex_);

    // 未使用のディスクリプタが要求量より少ない
    if (numDescriptors > numFreeHandles_) {
        return DescriptorAllocation();
    }

    // 要求を満たすのに十分な大きさのブロックを取得
    auto smallestBlockIter = freeListBySize_.lower_bound(numDescriptors);
    // 取得できなかった
    if (smallestBlockIter == freeListBySize_.end()) {
        return DescriptorAllocation();
    }

    // 最小ブロックのサイズ
    auto blockSize = smallestBlockIter->first;
    // 取得したブロックのオフセットへのイテレータ
    auto offsetIter = smallestBlockIter->second;
    // オフセット
    auto offset = offsetIter->first;

    // 既存のフリーブロックを削除
    freeListBySize_.erase(smallestBlockIter);
    freeListByOffset_.erase(offsetIter);

    // ブロックを分割し新しいブロックを計算
    auto newOffset = offset + numDescriptors;
    auto newSize = blockSize - numDescriptors;

    // 余りがある場合再びフリーリストに登録
    if (newSize > 0) {
        AddNewBlock(newOffset, newSize);
    }
    // 未使用ディスクリプタを減らす
    numFreeHandles_ -= numDescriptors;

    // 割り当てたディスクリプタを計算
    D3D12_CPU_DESCRIPTOR_HANDLE allocationHandle = baseDescriptor_;
    allocationHandle.ptr += size_t(descriptorSize_) * offset;

    return DescriptorAllocation(allocationHandle, numDescriptors, descriptorSize_, shared_from_this());
}

void DescriptorAllocatorPage::Free(DescriptorAllocation&& descriptor, uint64_t frameNumber) {
    auto offset = ComputeOffset(descriptor.GetDescriptorHandle());

    std::lock_guard<std::mutex> lock(allocationMutex_);
    // 解放待ち行列に追加
    staleDescriptors_.emplace(offset, descriptor.GetNumHandles(), frameNumber);
}

void DescriptorAllocatorPage::ReleaseStaleDescriptors(uint64_t frameNumber) {
    std::lock_guard<std::mutex> lock(allocationMutex_);

    while (!staleDescriptors_.empty() && staleDescriptors_.front().frameNumber <= frameNumber) {
        auto& staleDescriptor = staleDescriptors_.front();

        auto offset = staleDescriptor.offset;
        auto numDescriptors = staleDescriptor.size;

        FreeBlock(offset, numDescriptors);

        staleDescriptors_.pop();
    }
}

bool DescriptorAllocatorPage::HasSpace(uint32_t numDescriptors) const {
    return freeListBySize_.lower_bound(numDescriptors) != freeListBySize_.end();
}

uint32_t DescriptorAllocatorPage::ComputeOffset(D3D12_CPU_DESCRIPTOR_HANDLE handle) {
    return static_cast<uint32_t>(handle.ptr - baseDescriptor_.ptr) / descriptorSize_;
}

void DescriptorAllocatorPage::AddNewBlock(uint32_t offset, uint32_t numDescriptors) {
    auto offsetIter = freeListByOffset_.emplace(offset, numDescriptors);
    auto sizeIter = freeListBySize_.emplace(numDescriptors, offsetIter.first);
    offsetIter.first->second.freeListBySizeIter = sizeIter;
}

void DescriptorAllocatorPage::FreeBlock(uint32_t offset, uint32_t numDescriptors) {
    // 解放予定のブロックの次のブロックを検索
    auto nextBlockIter = freeListByOffset_.upper_bound(offset);

    auto prevBlokIter = nextBlockIter;
    // 先頭以外の場合一つ前
    if (prevBlokIter != freeListByOffset_.begin()) {
        --prevBlokIter;
    }
    // 先頭の場合末尾
    else {
        prevBlokIter = freeListByOffset_.end();
    }
    // 未使用ディスクリプタを戻す
    numFreeHandles_ += numDescriptors;

    // 直前のブロックの末尾と解放ブロックの先頭が等しい場合マージ
    if (prevBlokIter != freeListByOffset_.end() &&
        offset == prevBlokIter->first + prevBlokIter->second.size) {
        // マージ後のブロックに調整
        offset = prevBlokIter->first;
        numDescriptors += prevBlokIter->second.size;
        // マップから削除
        freeListBySize_.erase(prevBlokIter->second.freeListBySizeIter);
        freeListByOffset_.erase(prevBlokIter);
    }

    // 直後のブロックの先頭と解放ブロックの末尾が等しい場合マージ
    if (nextBlockIter != freeListByOffset_.end() &&
        offset + numDescriptors == nextBlockIter->first) {
        // マージ後のブロックに調整
        numDescriptors += nextBlockIter->second.size;
        // マップから削除
        freeListBySize_.erase(nextBlockIter->second.freeListBySizeIter);
        freeListByOffset_.erase(nextBlockIter);
    }
    // マージ後のブロックをリストに追加
    AddNewBlock(offset, numDescriptors);
}
