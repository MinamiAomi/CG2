#include "PixelBuffer.h"

#include "Graphics.h"
#include "Utility.h"

void PixelBuffer::Attach(const std::wstring& name, ID3D12Resource* resource, D3D12_RESOURCE_STATES currentState) {
    ASSERT(resource);

    Destory();

    auto resourceDesc = resource->GetDesc();

    resource_.Attach(resource);
    currentState_ = currentState;

    width_ = uint32_t(resourceDesc.Width);
    height_ = resourceDesc.Height;
    arraySize_ = resourceDesc.DepthOrArraySize;
    mipLevels_ = resourceDesc.MipLevels;
    format_ = resourceDesc.Format;

    ID3D12OBJECT_SET_NAME(resource_, name.c_str());
}

void PixelBuffer::CreateTextureResource(const std::wstring& name, const D3D12_RESOURCE_DESC& desc, const D3D12_CLEAR_VALUE& clearValue) {
    Destory();

    width_ = uint32_t(desc.Width);
    height_ = uint32_t(desc.Height);
    arraySize_ = uint16_t(desc.DepthOrArraySize);
    mipLevels_ = uint16_t(desc.MipLevels);
    format_ = desc.Format;

    D3D12_HEAP_PROPERTIES heapProperties{};
    heapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
    heapProperties.CreationNodeMask = 1;
    heapProperties.VisibleNodeMask = 1;

    ASSERT_SUCCEEDED(Graphics::GetInstance()->GetDevice()->CreateCommittedResource(
        &heapProperties, D3D12_HEAP_FLAG_NONE, &desc, 
        D3D12_RESOURCE_STATE_COMMON, &clearValue, IID_PPV_ARGS(resource_.GetAddressOf())));

    currentState_ = D3D12_RESOURCE_STATE_COMMON;
    gpuVirtualAddress_ = D3D12_GPU_VIRTUAL_ADDRESS_NULL;

    ID3D12OBJECT_SET_NAME(resource_, name.c_str());
}

