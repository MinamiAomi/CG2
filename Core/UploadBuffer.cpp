#include "UploadBuffer.h"

#include "../Externals/DirectXTex/d3dx12.h"

#include "Graphics.h"

void UploadBuffer::Create(const std::wstring& name, size_t bufferSize) {

    Destory();

    bufferSize_ = bufferSize;

    D3D12_HEAP_PROPERTIES heapProperties{};
    heapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
    heapProperties.CreationNodeMask = 1;
    heapProperties.VisibleNodeMask = 1;

    D3D12_RESOURCE_DESC resourceDesc = GetBufferDesc(bufferSize_);

    ASSERT_SUCCEEDED(Graphics::GetInstance()->GetDevice()->CreateCommittedResource(
        &heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(resource_.GetAddressOf())));

    currentState_ = D3D12_RESOURCE_STATE_GENERIC_READ;
    gpuVirtualAddress_ = resource_->GetGPUVirtualAddress();

    ID3D12OBJECT_SET_NAME(resource_, name.c_str());
}

void* UploadBuffer::Map() {
    void* dataBegin = nullptr;
    resource_->Map(0, nullptr, &dataBegin);
    return dataBegin;
}

void UploadBuffer::Unmap() {
    resource_->Unmap(0, nullptr);
}

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
    auto device = Graphics::GetInstance()->GetDevice();
    
    ASSERT_SUCCEEDED(device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(pageSize_),
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(resource_.GetAddressOf())));

    gpuPtr_ = resource_->GetGPUVirtualAddress();
    resource_->Map(0, nullptr, &cpuPtr_);
}

UploadBuffer::Page::~Page() {
    resource_->Unmap(0, nullptr);
    cpuPtr_ = nullptr;
    gpuPtr_ = D3D12_GPU_VIRTUAL_ADDRESS_NULL;
}

bool UploadBuffer::Page::HasSpace(size_t sizeInByte, size_t alignment) const {
    return false;
}

UploadBuffer::Allocation UploadBuffer::Page::Allocate(size_t sizeInByte, size_t alignment) {
    return Allocation();
}

void UploadBuffer::Page::Reset() {
}
