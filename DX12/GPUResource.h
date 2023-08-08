#pragma once

#include <d3d12.h>
#include <wrl/client.h>

#include <cstdint>

#ifndef D3D12_GPU_VIRTUAL_ADDRESS_NULL
#define D3D12_GPU_VIRTUAL_ADDRESS_NULL ((D3D12_GPU_VIRTUAL_ADDRESS)0)
#endif

class GPUResource {
public:
    ~GPUResource() { Destory(); }

    virtual void Destory() {
        resource_ = nullptr;
        gpuVirtualAddress_ = D3D12_GPU_VIRTUAL_ADDRESS_NULL;
    }

    ID3D12Resource* operator->() { return resource_.Get(); }
    const ID3D12Resource* operator->() const { return resource_.Get(); }

    ID3D12Resource* GetResource() { return resource_.Get(); }
    const ID3D12Resource* GetResource() const { return resource_.Get(); }

    ID3D12Resource** GetAddressOf() { return resource_.GetAddressOf(); }

    D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const { return gpuVirtualAddress_; }

protected:
    D3D12_RESOURCE_DESC GetBufferDesc(size_t bufferSize, D3D12_RESOURCE_FLAGS flags) {
        D3D12_RESOURCE_DESC desc{};
        desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        desc.Width = UINT64(bufferSize);
        desc.Height = 1;
        desc.DepthOrArraySize = 1;
        desc.MipLevels = 1;
        desc.SampleDesc.Count = 1;
        desc.Format = DXGI_FORMAT_UNKNOWN;
        desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
        desc.Flags = flags;
        return desc;
    }
    D3D12_RESOURCE_DESC GetTexture2DDesc(uint32_t width, uint32_t height, uint16_t arraySize, uint16_t mipLevels, DXGI_FORMAT format, D3D12_RESOURCE_FLAGS flags) {
        D3D12_RESOURCE_DESC desc{};
        desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        desc.Width = UINT64(width);
        desc.Height = height;
        desc.DepthOrArraySize = arraySize;
        desc.MipLevels = mipLevels;
        desc.Format = format;
        desc.SampleDesc.Count = 1;
        desc.Flags = flags;
        return desc;
    }

    Microsoft::WRL::ComPtr<ID3D12Resource> resource_;
    D3D12_RESOURCE_STATES currentState_ = D3D12_RESOURCE_STATE_COMMON;
    D3D12_GPU_VIRTUAL_ADDRESS gpuVirtualAddress_ = D3D12_GPU_VIRTUAL_ADDRESS_NULL;
};