#include "Resource.h"

#include <cassert>

#include "Device.h"

namespace CG::DX12 {

    void Resource::Initialize(
        const Device& device,
        HeapType heap,
        const D3D12_RESOURCE_DESC& resourceDesc,
        State initialState,
        const D3D12_CLEAR_VALUE* optimizedClearValue) {
        assert(device.IsEnabled());
        assert(heap != HeapType::Unknown);

        resource_.Reset();

        D3D12_HEAP_PROPERTIES heapProperties{};
        heapProperties.Type = static_cast<D3D12_HEAP_TYPE>(heap);

        if (FAILED(device.GetDevice()->CreateCommittedResource(
            &heapProperties,
            D3D12_HEAP_FLAG_NONE,
            &resourceDesc,
            static_cast<D3D12_RESOURCE_STATES>(initialState),
            optimizedClearValue,
            IID_PPV_ARGS(resource_.GetAddressOf())))) {
            assert(false);
        }
        currentState_ = initialState;
        heap_ = heap;
    }

    void Resource::InitializeForResource(
        ComPtr<ID3D12Resource> resource,
        State state) {
        assert(resource);
        D3D12_HEAP_PROPERTIES heapProperties{};
        D3D12_HEAP_FLAGS heapFlags{};
        if (FAILED(resource->GetHeapProperties(&heapProperties, &heapFlags))) {
            assert(false);
        }
        resource_ = resource;
        currentState_ = state;
        heap_ = static_cast<HeapType>(heapProperties.Type);
    }

    void Resource::InitializeForBuffer(
        const Device& device,
        size_t bufferSize,
        State initialState,
        HeapType heap,
        Flag flag) {
        assert(device.IsEnabled());
        assert(heap != HeapType::Unknown);

        D3D12_RESOURCE_DESC desc{};
        desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        desc.Width = bufferSize;
        desc.Height = 1;
        desc.DepthOrArraySize = 1;
        desc.MipLevels = 1;
        desc.SampleDesc.Count = 1;
        desc.Format = DXGI_FORMAT_UNKNOWN;
        desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
        desc.Flags = static_cast<D3D12_RESOURCE_FLAGS>(flag);

        Initialize(device, heap, desc, initialState);
    }

    void Resource::InitializeForTexture2D(
        const Device& device,
        uint64_t width,
        uint32_t height,
        uint8_t depthOrArraySize,
        uint8_t mipLevels,
        DXGI_FORMAT format,
        State initialState,
        HeapType heap,
        Flag flag,
        const float* clearColor) {
        assert(device.IsEnabled());
        assert(heap != HeapType::Unknown);

        D3D12_RESOURCE_DESC desc{};
        desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        desc.Width = width;
        desc.Height = height;
        desc.DepthOrArraySize = depthOrArraySize;
        desc.MipLevels = mipLevels;
        desc.SampleDesc.Count = 1;
        desc.Format = format;
        desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
        desc.Flags = static_cast<D3D12_RESOURCE_FLAGS>(flag);

        D3D12_CLEAR_VALUE* cvptr = nullptr;
        D3D12_CLEAR_VALUE clearValue{};
        if ((static_cast<uint8_t>(flag) & static_cast<uint8_t>(Flag::AllowRenderTarget))) {
            if (clearColor) {
                cvptr = &clearValue;
                clearValue.Format = desc.Format;
                memcpy(clearValue.Color, clearColor, sizeof(clearValue.Color));
            }
        }
        if ((static_cast<uint8_t>(flag) & static_cast<uint8_t>(Flag::AllowDepthStencil))) {
            cvptr = &clearValue;
            clearValue.Format = desc.Format;
            clearValue.DepthStencil.Depth = 1.0f;
            clearValue.DepthStencil.Stencil = 0;
        }
        Initialize(device, heap, desc, initialState, cvptr);
    }

    D3D12_RESOURCE_BARRIER Resource::TransitionBarrier(State afterState) {
        assert(IsEnabled());

        D3D12_RESOURCE_BARRIER barrier{};
        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Transition.pResource = resource_.Get();
        barrier.Transition.StateBefore = static_cast<D3D12_RESOURCE_STATES>(currentState_);
        barrier.Transition.StateAfter = static_cast<D3D12_RESOURCE_STATES>(afterState);
        currentState_ = afterState;

        return barrier;
    }

    D3D12_RESOURCE_BARRIER Resource::UAVBarrier() {
        assert(IsEnabled());
        assert(currentState_ == State::UnorderedAccess);

        D3D12_RESOURCE_BARRIER barrier{};
        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
        barrier.UAV.pResource = resource_.Get();

        return barrier;
    }

    void* Resource::Map() {
        assert(IsEnabled());
        assert(heap_ != HeapType::Default);

        void* mappedPtr = nullptr;
        resource_->Map(0, nullptr, &mappedPtr);

        return mappedPtr;
    }

    void Resource::Unmap() {
        assert(IsEnabled());
        assert(heap_ != HeapType::Default);

        resource_->Unmap(0, nullptr);
    }

}