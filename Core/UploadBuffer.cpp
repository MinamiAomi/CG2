#include "UploadBuffer.h"

#include "../Externals/DirectXTex/d3dx12.h"

#include "Defines.h"
#include "Graphics.h"
#include "Utility.h"

UploadBuffer::UploadBuffer(size_t pageSize) :
    pageSize_(pageSize) {
}

UploadBuffer::~UploadBuffer() {
}

UploadBuffer::Allocation UploadBuffer::Allocate(size_t sizeInByte, size_t alignment) {
    ASSERT(sizeInByte <= pageSize_);

    if (!currentPage_ || !currentPage_->HasSpace(sizeInByte, alignment)) {
        currentPage_ = RequestPage();
    }

    return currentPage_->Allocate(sizeInByte, alignment);
}

void UploadBuffer::Reset() {
    currentPage_ = nullptr;
    availablePages_ = pagePool_;

    for (auto& page : availablePages_) {
        page->Reset();
    }
}

std::shared_ptr<UploadBuffer::Page> UploadBuffer::RequestPage() {
    std::shared_ptr<Page> page;

    if (!availablePages_.empty()) {
        page = availablePages_.front();
        availablePages_.pop_front();
    }
    else {
        page = std::make_shared<Page>(pageSize_);
        pagePool_.push_back(page);
    }

    return page;
}

UploadBuffer::Page::Page(size_t sizeInByte) :
    pageSize_(sizeInByte),
    offset_(0),
    cpuPtr_(nullptr),
    gpuPtr_(D3D12_GPU_VIRTUAL_ADDRESS_NULL) {
    auto device = Graphics::Get().GetDevice();

    ASSERT_IF_FAILED(device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(pageSize_),
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(resource_.GetAddressOf())));

    gpuPtr_ = resource_->GetGPUVirtualAddress();
    resource_->Map(0, nullptr, &cpuPtr_);

    NAME_D3D12_OBJECT(resource_);
}

UploadBuffer::Page::~Page() {
    resource_->Unmap(0, nullptr);
    cpuPtr_ = nullptr;
    gpuPtr_ = D3D12_GPU_VIRTUAL_ADDRESS_NULL;
}

bool UploadBuffer::Page::HasSpace(size_t sizeInByte, size_t alignment) const {
    size_t alignedSize = Utility::AlignUp(sizeInByte, alignment);
    size_t alignedOffset = Utility::AlignUp(offset_, alignment);

    return alignedOffset + alignedSize <= pageSize_;
}

UploadBuffer::Allocation UploadBuffer::Page::Allocate(size_t sizeInByte, size_t alignment) {
    ASSERT(HasSpace(sizeInByte, alignment));

    size_t alignedSize = Utility::AlignUp(sizeInByte, alignment);
    offset_ = Utility::AlignUp(offset_, alignment);

    Allocation allocation{};
    allocation.cpu = static_cast<uint8_t*>(cpuPtr_) + offset_;
    allocation.gpu = gpuPtr_ + offset_;

    offset_ += alignedSize;
    return allocation;
}

void UploadBuffer::Page::Reset() {
    offset_ = 0;
}
