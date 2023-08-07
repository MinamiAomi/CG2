#pragma once
#include "GPUResource.h"

#include <cstdint>

class TextureResource : public GPUResource {
public:
    TextureResource();

    void Create();

    uint32_t GetWidth() const { return width_; }
    uint32_t GetHeight() const { return height_; }
    uint16_t GetArraySize() const { return arraySize_; }
    uint16_t GetMipLevels() const { return mipLevels_; }
    DXGI_FORMAT GetFormat() const { return format_; }

protected:
    uint32_t width_;
    uint32_t height_;
    uint16_t arraySize_;
    uint16_t mipLevels_;
    DXGI_FORMAT format_;
};