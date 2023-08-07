#pragma once

#include <d3d12.h>
#include <wrl/client.h>

#ifndef D3D12_GPU_VIRTUAL_ADDRESS_NULL
#define D3D12_GPU_VIRTUAL_ADDRESS_NULL ((D3D12_GPU_VIRTUAL_ADDRESS)0)
#endif

class GPUResource {
public:
    GPUResource() :
        currentState_(D3D12_RESOURCE_STATE_COMMON),
        gpuVirtualAddress_(D3D12_GPU_VIRTUAL_ADDRESS_NULL) {
    }

    GPUResource(ID3D12Resource* resource, D3D12_RESOURCE_STATES currentState) :
        resource_(resource),
        currentState_(currentState),
        gpuVirtualAddress_(resource ? resource->GetGPUVirtualAddress() : D3D12_GPU_VIRTUAL_ADDRESS_NULL) {
    }

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
    Microsoft::WRL::ComPtr<ID3D12Resource> resource_;
    D3D12_RESOURCE_STATES currentState_;
    D3D12_GPU_VIRTUAL_ADDRESS gpuVirtualAddress_;
};