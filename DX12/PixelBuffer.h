#pragma once
#include "GPUResource.h"

#include <cstdint>
#include <string>

class PixelBuffer : public GPUResource {
public:

    uint32_t GetWidth() const { return width_; }
    uint32_t GetHeight() const { return height_; }
    uint16_t GetArraySize() const { return arraySize_; }
    uint16_t GetMipLevels() const { return mipLevels_; }
    DXGI_FORMAT GetFormat() const { return format_; }

protected:
    void Attach(const std::wstring& name, ID3D12Resource* resource, D3D12_RESOURCE_STATES currentState);

    void CreateTextureResource(const std::wstring& name, const D3D12_RESOURCE_DESC& desc, const D3D12_CLEAR_VALUE& clearValue);

    uint32_t width_ = 0;
    uint32_t height_ = 0;
    uint16_t arraySize_ = 0;
    uint16_t mipLevels_ = 0;
    DXGI_FORMAT format_ = DXGI_FORMAT_UNKNOWN;
};