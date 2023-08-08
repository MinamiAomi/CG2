#include "ColorBuffer.h"

#include "Graphics.h"

void ColorBuffer::CreateFromSwapChain(const std::wstring& name, ID3D12Resource* resource) {
    Attach(name, resource, D3D12_RESOURCE_STATE_PRESENT);
    
    auto graphics = Graphics::GetInstance();
    if (rtvHandle_.ptr == 0) {
        rtvHandle_ = graphics->AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    }
    graphics->GetDevice()->CreateRenderTargetView(resource_.Get(), nullptr, rtvHandle_);
}

void ColorBuffer::Create(const std::wstring& name, uint32_t width, uint32_t height, DXGI_FORMAT format, uint16_t arraySize) {
    auto resourceDesc = GetTexture2DDesc(width, height, arraySize, 1, format, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

    D3D12_CLEAR_VALUE clearColor{};
    clearColor.Format = format;
    memcpy(clearColor.Color, clearColor_, sizeof(clearColor.Color));

    CreateTextureResource(name, resourceDesc, clearColor);
    CreateViews();
}

void ColorBuffer::CreateViews() {
    auto graphics = Graphics::GetInstance();

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Format = format_;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

    D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
    rtvDesc.Format = format_;

    if (arraySize_ > 1) {
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
        srvDesc.Texture2DArray.ArraySize = UINT(arraySize_);
        srvDesc.Texture2DArray.MipLevels = UINT(mipLevels_);

        rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
        rtvDesc.Texture2DArray.ArraySize = UINT(arraySize_);
    }
    else {
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = UINT(mipLevels_);

        rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
    }

    if (srvHandle_.ptr == 0) {
        srvHandle_ = graphics->AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
        rtvHandle_ = graphics->AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    }

    graphics->GetDevice()->CreateShaderResourceView(resource_.Get(), &srvDesc, srvHandle_);
    graphics->GetDevice()->CreateRenderTargetView(resource_.Get(), &rtvDesc, rtvHandle_);
}
