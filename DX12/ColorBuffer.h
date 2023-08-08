#pragma once
#include "PixelBuffer.h"

#include <string>

class ColorBuffer : public PixelBuffer {
public:
    void CreateFromSwapChain(const std::wstring& name, ID3D12Resource* resource);
    void Create(const std::wstring& name, uint32_t width, uint32_t height, DXGI_FORMAT format, uint16_t arraySize = 1);

    const D3D12_CPU_DESCRIPTOR_HANDLE& GetSRV() const { return srvHandle_; }
    const D3D12_CPU_DESCRIPTOR_HANDLE& GetRTV() const { return rtvHandle_; }

    void SetClearColor(const float* clearColor) { memcpy(clearColor_, clearColor, sizeof(clearColor_)); }

private:
    void CreateViews();

    D3D12_CPU_DESCRIPTOR_HANDLE srvHandle_{ 0 };
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle_{ 0 };
    float clearColor_[4]{ 0.0f };
};
