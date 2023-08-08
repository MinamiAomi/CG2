#pragma once
#include "PixelBuffer.h"

#include <string>

class DepthBuffer : public PixelBuffer {
public:
    void Create(const std::wstring& name, uint32_t width, uint32_t height, DXGI_FORMAT format);

    const D3D12_CPU_DESCRIPTOR_HANDLE& GetDSV() { return dsvHandle_; }
    const D3D12_CPU_DESCRIPTOR_HANDLE& GetReadOnlyDSV() { return readOnlyDSVHandle_; }
    const D3D12_CPU_DESCRIPTOR_HANDLE& GetSRV() { return srvHandle_; }

private:
    void CreateViews();

    D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle_{ 0 };
    D3D12_CPU_DESCRIPTOR_HANDLE readOnlyDSVHandle_{ 0 };
    D3D12_CPU_DESCRIPTOR_HANDLE srvHandle_{ 0 };
};