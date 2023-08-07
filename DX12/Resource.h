#pragma once

#include <d3d12.h>
#include <wrl/client.h>

#include <cstdint>

namespace CG::DX12 {
    using namespace Microsoft::WRL;

    class Device;

    class Resource {
    public:
        enum class HeapType {
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
        enum class State {
            Common = D3D12_RESOURCE_STATE_COMMON,
            VertexAndConstantBuffer = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
            IndexBuffer = D3D12_RESOURCE_STATE_INDEX_BUFFER,
            RenderTarget = D3D12_RESOURCE_STATE_RENDER_TARGET,
            UnorderedAccess = D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
            DepthWrite = D3D12_RESOURCE_STATE_DEPTH_WRITE,
            DepthRead = D3D12_RESOURCE_STATE_DEPTH_READ,
            NonPixelShaderResource = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,
            PixelShaderResource = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
            StreamOut = D3D12_RESOURCE_STATE_STREAM_OUT,
            IndirectArgument = D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT,
            CopyDest = D3D12_RESOURCE_STATE_COPY_DEST,
            CopySource = D3D12_RESOURCE_STATE_COPY_SOURCE,
            ResolveDest = D3D12_RESOURCE_STATE_RESOLVE_DEST,
            ResolveSource = D3D12_RESOURCE_STATE_RESOLVE_SOURCE,
            RaytracingAccelerationStructure = D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE,
            ShadingRateSource = D3D12_RESOURCE_STATE_SHADING_RATE_SOURCE,
            GenericRead = D3D12_RESOURCE_STATE_GENERIC_READ,
            AllShaderResource = D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE,
            Present = D3D12_RESOURCE_STATE_PRESENT,
            Predication = D3D12_RESOURCE_STATE_PREDICATION,
            VideoDecodeRead = D3D12_RESOURCE_STATE_VIDEO_DECODE_READ,
            VideoDecodeWrite = D3D12_RESOURCE_STATE_VIDEO_DECODE_WRITE,
            VideoProcessRead = D3D12_RESOURCE_STATE_VIDEO_PROCESS_READ,
            VideoProcessWrite = D3D12_RESOURCE_STATE_VIDEO_PROCESS_WRITE,
            VideoEncodeRead = D3D12_RESOURCE_STATE_VIDEO_ENCODE_READ,
            VideoEncodeWrite = D3D12_RESOURCE_STATE_VIDEO_ENCODE_WRITE
        };

        void Initialize(
            const Device& device,
            HeapType heap,
            const D3D12_RESOURCE_DESC& resourceDesc,
            State initialState,
            const D3D12_CLEAR_VALUE* optimizedClearValue = nullptr);

        void InitializeForResource(
            ComPtr<ID3D12Resource> resource,
            State state = State::Common);

        void InitializeForBuffer(
            const Device& device,
            size_t bufferSize,
            State initialState = State::GenericRead,
            HeapType heap = HeapType::Upload,
            Flag flag = Flag::None);

        void InitializeForTexture2D(
            const Device& device,
            uint64_t width,
            uint32_t height,
            uint8_t depthOrArraySize,
            uint8_t mipLevels,
            DXGI_FORMAT format,
            State initialState = State::GenericRead,
            HeapType heap = HeapType::Default,
            Flag flag = Flag::None,
            const float* clearColor = nullptr);


        D3D12_RESOURCE_BARRIER TransitionBarrier(State afterState);
        D3D12_RESOURCE_BARRIER UAVBarrier();

        void* Map();
        template<class T>
        T* Map() { return reinterpret_cast<T*>(Map()); }
        void Unmap();

        bool IsEnabled() const { return resource_; }
        ComPtr<ID3D12Resource> GetResource() const { return resource_; }
        D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const { return resource_->GetGPUVirtualAddress(); }
        State GetState() const { return currentState_; }

    private:
        ComPtr<ID3D12Resource> resource_;
        State currentState_{ State::Common };
        HeapType heap_{ HeapType::Unknown };
        Flag flag_{ Flag::None };
    };

}

