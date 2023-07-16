#pragma once

#include <d3d12.h>
#include <wrl/client.h>

#include <cstdint>

namespace CG::DX12 {
    using namespace Microsoft::WRL;

    class Device;

    class Resource {
    public:
        enum class Heap {
            Unknown = 0,
            Default = D3D12_HEAP_TYPE_DEFAULT,
            Upload = D3D12_HEAP_TYPE_UPLOAD,
            ReadBack = D3D12_HEAP_TYPE_READBACK,
        };
        enum class Flag {
            None = D3D12_RESOURCE_FLAG_NONE,
            AllowRenderTarget = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET,
            AllowDepthStencil = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL,
            AllowUnorderedAccess = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
        };

        Resource();

        void Initialize(
            const Device& device,
            Heap heap,
            const D3D12_RESOURCE_DESC& resourceDesc,
            D3D12_RESOURCE_STATES initialResourceState,
            const D3D12_CLEAR_VALUE* optimizedClearValue = nullptr);

        void InitializeForResource(
            ComPtr<ID3D12Resource> resource,
            D3D12_RESOURCE_STATES state = D3D12_RESOURCE_STATE_COMMON);

        void InitializeForBuffer(
            const Device& device,
            Heap heap,
            size_t bufferSize,
            D3D12_RESOURCE_STATES initialResourceState = D3D12_RESOURCE_STATE_GENERIC_READ,
            Flag flag = Flag::None);

        void InitializeForTexture2D(
            const Device& device,
            Heap heap,
            uint64_t width,
            uint32_t height,
            uint8_t depthOrArraySize,
            uint8_t mipLevels,
            DXGI_FORMAT format,
            D3D12_RESOURCE_STATES initialResourceState = D3D12_RESOURCE_STATE_GENERIC_READ,
            Flag flag = Flag::None,
            const float* clearColor = nullptr);


        D3D12_RESOURCE_BARRIER TransitionBarrier(D3D12_RESOURCE_STATES afterState);
        D3D12_RESOURCE_BARRIER UAVBarrier();

        void* Map();
        template<class T>
        T* Map() { return reinterpret_cast<T*>(Map()); }
        void Unmap();

        bool IsEnabled() const { return resource_; }
        ComPtr<ID3D12Resource> GetResource() const { return resource_; }
        D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const { return resource_->GetGPUVirtualAddress(); }
        D3D12_RESOURCE_STATES GetState() const { return currentState_; }

    private:
        ComPtr<ID3D12Resource> resource_;
        D3D12_RESOURCE_STATES currentState_;
        Heap heap_;
        Flag flag_;
    };

}