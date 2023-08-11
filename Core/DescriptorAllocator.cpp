#include "DescriptorAllocator.h"

#include "../Externals/DirectXTex/d3dx12.h"

#include "Graphics.h"
#include "DescriptorAllocatorPage.h"

DescriptorAllocator::DescriptorAllocator(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptorsPerHeap) :
    heapType_(type),
    numDescriptorsPerHeap_(numDescriptorsPerHeap) {
}

DescriptorAllocator::~DescriptorAllocator() {
}

DescriptorAllocation DescriptorAllocator::Allocate(uint32_t numDescriptors) {
    std::lock_guard<std::mutex> lock(allocationMutex_);

    DescriptorAllocation allocation;
    // 利用可能なヒープから割り当てる
    for (auto iter = availableHeaps_.begin(); iter != availableHeaps_.end(); ++iter) {
        auto allocatorPage = heapPool_[*iter];

        allocation = allocatorPage->Allocate(numDescriptors);

        if (allocatorPage->GetNumFreeHandles() == 0) {
            iter = availableHeaps_.erase(iter);
        }
        // 割り当てられた
        if (!allocation.IsNull()) {
            break;
        }
    }

    // 割り当てることができなかった場合
    // 新しいヒープを作成し割り当てる
    if (allocation.IsNull()) {
        numDescriptorsPerHeap_ = std::max(numDescriptorsPerHeap_, numDescriptors);
        auto newPage = CreateAllocatorPage();

        allocation = newPage->Allocate(numDescriptors);
    }

    return allocation;
}

void DescriptorAllocator::ReleaseStaleDescriptors(uint64_t frameNumber) {
    std::lock_guard<std::mutex> lock(allocationMutex_);

    for (size_t i = 0; i < heapPool_.size(); ++i) {
        auto page = heapPool_[i];
        
        page->ReleaseStaleDescriptors(frameNumber);
        if (page->GetNumFreeHandles() > 0) {
            availableHeaps_.insert(i);
        }
    }
}

std::shared_ptr<DescriptorAllocatorPage> DescriptorAllocator::CreateAllocatorPage() {
    auto newPage = std::make_shared<DescriptorAllocatorPage>(heapType_, numDescriptorsPerHeap_);

    heapPool_.emplace_back(newPage);
    availableHeaps_.insert(heapPool_.size() - 1);

    return newPage;
}
