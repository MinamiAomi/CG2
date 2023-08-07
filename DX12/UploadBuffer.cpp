#include "UploadBuffer.h"

#include "Utility.h"
#include "Graphics.h"

void UploadBuffer::Create(const std::wstring& name, size_t bufferSize) {

    Destory();

    bufferSize_ = bufferSize;

    D3D12_HEAP_PROPERTIES heapProperties{};
    heapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
    heapProperties.CreationNodeMask = 1;
    heapProperties.VisibleNodeMask = 1;

    D3D12_RESOURCE_DESC resourceDesc{};
    resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    resourceDesc.Width = UINT64(bufferSize);
    resourceDesc.Height = 1;
    resourceDesc.DepthOrArraySize = 1;
    resourceDesc.MipLevels = 1;
    resourceDesc.SampleDesc.Count = 1;
    resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
    resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

    ASSERT_SUCCEEDED(Graphics::GetInstance()->GetDevice()->CreateCommittedResource(
        &heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc, 
        D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(resource_.GetAddressOf())));

    currentState_ = D3D12_RESOURCE_STATE_GENERIC_READ;
    gpuVirtualAddress_ = resource_->GetGPUVirtualAddress();

#ifdef _DEBUG
    resource_->SetName(name.c_str());
#endif // _DEBUG
}

void* UploadBuffer::Map() {
    void* dataBegin = nullptr;
    resource_->Map(0, nullptr, &dataBegin);
    return dataBegin;
}

void UploadBuffer::Unmap() {
    resource_->Unmap(0, nullptr);
}
