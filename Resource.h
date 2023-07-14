#pragma once

#include <d3d12.h>
#include <wrl/client.h>

namespace CG::DX12 {

    class Resource {
    public:
        Resource();

        void CreateCommittedResource(
            Microsoft::WRL::ComPtr<ID3D12Device> device,
            const D3D12_HEAP_PROPERTIES& heapProperties,
            const D3D12_RESOURCE_DESC& resourceDesc,
            D3D12_RESOURCE_STATES initialResourceState,
            const D3D12_CLEAR_VALUE& optimizedClearValue = {});

        D3D12_RESOURCE_BARRIER TransitionBarrier(D3D12_RESOURCE_STATES afterState);
        D3D12_RESOURCE_BARRIER UAVBarrier();

        bool IsEnabled() const { return resource_; }
        Microsoft::WRL::ComPtr<ID3D12Resource> GetComPtr() const { return resource_; }
        ID3D12Resource* GetRawPtr() const { return resource_.Get(); }
        D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const { return resource_->GetGPUVirtualAddress(); }
        D3D12_RESOURCE_STATES GetState() const { return state_; }

    private:
        Microsoft::WRL::ComPtr<ID3D12Resource> resource_;
        D3D12_RESOURCE_STATES state_;
    };

}