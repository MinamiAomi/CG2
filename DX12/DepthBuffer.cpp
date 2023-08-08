#include "DepthBuffer.h"

#include "Graphics.h"

void DepthBuffer::Create(const std::wstring& name, uint32_t width, uint32_t height, DXGI_FORMAT format) {
    auto desc = GetTexture2DDesc(width, height, 1, 1, format, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
   
    D3D12_CLEAR_VALUE clearValue{};
    clearValue.Format = format;
    clearValue.DepthStencil.Depth = 1.0f;
    clearValue.DepthStencil.Stencil = 0;

    CreateTextureResource(name, desc, clearValue);
    CreateViews();
}

void DepthBuffer::CreateViews() {
    auto graphics = Graphics::GetInstance();

    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
    dsvDesc.Format = format_;
    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;

    if (dsvHandle_.ptr == 0) {
        dsvHandle_ = graphics->AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
        readOnlyDSVHandle_ = graphics->AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
    }

    dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
    graphics->GetDevice()->CreateDepthStencilView(resource_.Get(), &dsvDesc, dsvHandle_);

    dsvDesc.Flags = D3D12_DSV_FLAG_READ_ONLY_DEPTH;
    graphics->GetDevice()->CreateDepthStencilView(resource_.Get(), &dsvDesc, readOnlyDSVHandle_);

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Format = format_;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;

    if (srvHandle_.ptr == 0) {
        srvHandle_ = graphics->AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    }

    graphics->GetDevice()->CreateShaderResourceView(resource_.Get(), &srvDesc, srvHandle_);
}
