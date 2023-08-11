#include "DescriptorAllocation.h"

#include "Defines.h"
#include "Graphics.h"

#include "DescriptorAllocatorPage.h"

DescriptorAllocation::DescriptorAllocation() :
    descriptor_(D3D12_CPU_DESCRIPTOR_HANDLE_NULL),
    numHandles_(0),
    descriptorSize_(0) {
}

DescriptorAllocation::DescriptorAllocation(D3D12_CPU_DESCRIPTOR_HANDLE descriptor, uint32_t numHandles, uint32_t descriptorSize, std::shared_ptr<DescriptorAllocatorPage> page) :
    descriptor_(descriptor),
    numHandles_(numHandles),
    descriptorSize_(descriptorSize),
    page_(page) {
}

DescriptorAllocation::~DescriptorAllocation() {
    Free();
}

DescriptorAllocation::DescriptorAllocation(DescriptorAllocation&& move) noexcept :
    descriptor_(move.descriptor_),
    numHandles_(move.numHandles_),
    descriptorSize_(move.descriptorSize_),
    page_(std::move(page_)) {
    move.descriptor_ = D3D12_CPU_DESCRIPTOR_HANDLE_NULL;
    move.numHandles_ = 0;
    move.descriptorSize_ = 0;
}

DescriptorAllocation& DescriptorAllocation::operator=(DescriptorAllocation&& move) noexcept {
    // 保持している場合解放する
    Free();

    descriptor_ = move.descriptor_;
    numHandles_ = move.numHandles_;
    descriptorSize_ = move.descriptorSize_;
    page_ = std::move(move.page_);

    move.descriptor_ = D3D12_CPU_DESCRIPTOR_HANDLE_NULL;
    move.numHandles_ = 0;
    move.descriptorSize_ = 0;

    return *this;
}

D3D12_CPU_DESCRIPTOR_HANDLE DescriptorAllocation::GetDescriptorHandle(uint32_t offset) const {
    ASSERT(offset < numHandles_);
    return { descriptor_.ptr + (descriptorSize_ * offset) };
}

void DescriptorAllocation::Free() {
    if (!IsNull() && page_) {
        page_->Free(std::move(*this), Graphics::GetFrameCount());

        descriptor_.ptr = 0;
        numHandles_ = 0;
        descriptorSize_ = 0;
        page_.reset();
    }
}
