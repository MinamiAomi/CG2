#include "Resource.h"

#include <cassert>

namespace CG::DX12 {

    Resource::Resource() :
        state_(D3D12_RESOURCE_STATE_COMMON) {
    }

    void Resource::CreateCommittedResource(
        Microsoft::WRL::ComPtr<ID3D12Device> device,
        const D3D12_HEAP_PROPERTIES& heapProperties,
        const D3D12_RESOURCE_DESC& resourceDesc,
        D3D12_RESOURCE_STATES initialResourceState,
        const D3D12_CLEAR_VALUE& optimizedClearValue) {
        assert(device);

        if (FAILED(device->CreateCommittedResource(
            &heapProperties,
            D3D12_HEAP_FLAG_NONE,
            &resourceDesc,
            initialResourceState,
            &optimizedClearValue,
            IID_PPV_ARGS(resource_.ReleaseAndGetAddressOf())))) {
            assert(false);
        }
        state_ = initialResourceState;
    }

    D3D12_RESOURCE_BARRIER Resource::TransitionBarrier(D3D12_RESOURCE_STATES afterState) {
        assert(IsEnabled());
        D3D12_RESOURCE_BARRIER barrier{};
        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Transition.pResource = resource_.Get();
        barrier.Transition.StateBefore = state_;
        barrier.Transition.StateAfter = afterState;
        state_ = afterState;
        return barrier;
    }

    D3D12_RESOURCE_BARRIER Resource::UAVBarrier() {
        assert(IsEnabled());
        D3D12_RESOURCE_BARRIER barrier{};
        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
        barrier.UAV.pResource = resource_.Get();
        assert(state_ == D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
        return barrier;
    }

}