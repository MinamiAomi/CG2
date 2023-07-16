#pragma once

#include <d3d12.h>
#include <wrl/client.h>

#include <cstdint>

#include "DescriptorHeap.h"

namespace CG::DX12 {
    using namespace Microsoft::WRL;

    class Device;
    class Resource;

    class VertexBufferView {
    public:
        void Initialize(
            const Resource& resource,
            size_t bufferSize,
            size_t strideSize,
            size_t vertexCount,
            int32_t baseVertexLocation = 0);

        bool IsEnabled() const { return view_.BufferLocation != 0; }
        const D3D12_VERTEX_BUFFER_VIEW& GetView() const { return view_; }
        int32_t GetBaseVertexLocation() const { return baseVertexLocation_; }
        const uint32_t GetVertexCount() const { return vertexCount_; }

    private:
        D3D12_VERTEX_BUFFER_VIEW view_{};
        int32_t baseVertexLocation_{ 0 };
        uint32_t vertexCount_{ 0 };
    };

    class IndexBufferView {
    public:
        enum Format {
            UInt16 = DXGI_FORMAT_R16_UINT,
            UInt32 = DXGI_FORMAT_R32_UINT
        };

        void Initialize(
            const Resource& resurce,
            size_t bufferSize,
            size_t indexCount,
            Format format = UInt32,
            int32_t startIndexLocation = 0);

        bool IsEnabled() const { return view_.BufferLocation; }
        const D3D12_INDEX_BUFFER_VIEW& GetView() const { return view_; }
        uint32_t GetStartIndexLocation() const { return startIndexLocation_; }
        uint32_t GetIndexCount() const { return indexCount_; }

    private:
        D3D12_INDEX_BUFFER_VIEW view_{};
        uint32_t startIndexLocation_{ 0 };
        uint32_t indexCount_{ 0 };
    };

    class ConstantBufferView {
    public:
        void Initialize(
            const Device& device,
            const Resource& resource,
            size_t bufferSize,
            const Descriptor& descriptor);

        bool IsEnabled() const { return descriptor_.IsEnabled(); }
        const Descriptor& GetDescriptor() const { return descriptor_; }

    private:
        Descriptor descriptor_;
    };

    class ShaderResourceView {
    public:
        void InitializeStructuredBuffer(
            const Device& device,
            const Resource& resource,
            uint32_t firstElement,
            uint32_t elementCount,
            uint32_t strideSize,
            const Descriptor& descriptor);

        void InitializeByteAddressBuffer(
            const Device& device,
            const Resource& resource,
            uint32_t firstElement,
            uint32_t elementCount,
            const Descriptor& descriptor);

        void InitializeTexture2D(
            const Device& device,
            const Resource& resource,
            const Descriptor& descriptor);

        bool IsEnabled() const { return descriptor_.IsEnabled(); }
        const Descriptor& GetDescriptor() const { return descriptor_; }
        const D3D12_SHADER_RESOURCE_VIEW_DESC& GetDesc() { return desc_; }

    private:
        Descriptor descriptor_;
        D3D12_SHADER_RESOURCE_VIEW_DESC desc_{};
    };

    class UnorderedAccessView {
    public:
        void InitializeStructuredBuffer(
            const Device& device,
            const Resource& resource,
            uint32_t firstElement,
            uint32_t elementCount,
            uint32_t strideSize,
            const Descriptor& descriptor);

        void InitializeByteAddressBuffer(
            const Device& device,
            const Resource& resource,
            uint32_t firstElement,
            uint32_t elementCount,
            const Descriptor& descriptor);

        void InitializeTexture2D(
            const Device& device,
            const Resource& resource,
            const Descriptor& descriptor);

        bool IsEnabled() const { return descriptor_.IsEnabled(); }
        const Descriptor& GetDescriptor() const { return descriptor_; }
        const D3D12_UNORDERED_ACCESS_VIEW_DESC& GetDesc() { return desc_; }

    private:
        Descriptor descriptor_;
        D3D12_UNORDERED_ACCESS_VIEW_DESC desc_;
    };

    class RenderTargetView {
    public:
        // formatがUnknownの場合ResourceDescから参照
        void Initialize(
            const Device& device,
            const Resource& resource,
            const Descriptor& descriptor,
            DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN);

        bool IsEnabled() const { return descriptor_.IsEnabled(); }
        const Descriptor& GetDescriptor() const { return descriptor_; }
        const D3D12_RENDER_TARGET_VIEW_DESC& GetDesc() { return desc_; }

    private:
        Descriptor descriptor_;
        D3D12_RENDER_TARGET_VIEW_DESC desc_{};
    };

    class DepthStencilView {
    public:
        void Initialize(
            const Device& device,
            const Resource& resource,
            const Descriptor& descriptor);

        bool IsEnabled() const { return descriptor_.IsEnabled(); }
        const Descriptor& GetDescriptor() const { return descriptor_; }
        const  D3D12_DEPTH_STENCIL_VIEW_DESC& GetDesc() { return desc_; }

    private:
        Descriptor descriptor_;
        D3D12_DEPTH_STENCIL_VIEW_DESC desc_{};
    };

}