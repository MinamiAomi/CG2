#include "Views.h"

#include <cassert>

#include "Device.h"
#include "Resource.h"

namespace CG::DX12 {

    void VertexBufferView::Initialize(
        const Resource& resource,
        size_t bufferSize,
        size_t strideSize,
        size_t vertexCount,
        int32_t baseVertexLocation) {
        assert(resource.IsEnabled());
        assert(bufferSize > 0);
        assert(strideSize > 0);
        assert(vertexCount > 0);

        view_.BufferLocation = resource.GetGPUVirtualAddress();
        view_.SizeInBytes = static_cast<uint32_t>(bufferSize);
        view_.StrideInBytes = static_cast<uint32_t>(strideSize);
        baseVertexLocation_ = baseVertexLocation;
        vertexCount_ = static_cast<uint32_t>(vertexCount);
    }

    void IndexBufferView::Initialize(
        const Resource& resource,
        size_t bufferSize,
        size_t indexCount,
        Format format,
        int32_t startIndexLocation) {
        assert(resource.IsEnabled());
        assert(bufferSize > 0);
        assert(indexCount > 0);

        view_.BufferLocation = resource.GetGPUVirtualAddress();
        view_.SizeInBytes = static_cast<uint32_t>(bufferSize);
        view_.Format = static_cast<DXGI_FORMAT>(format);
        startIndexLocation_ = startIndexLocation;
        indexCount_ = static_cast<uint32_t>(indexCount);
    }

    void ConstantBufferView::Initialize(
        const Device& device,
        const Resource& resource,
        size_t bufferSize,
        const Descriptor& descriptor) {
        assert(device.IsEnabled());
        assert(resource.IsEnabled());
        assert(bufferSize > 0);
        assert(descriptor.IsEnabled());

        D3D12_CONSTANT_BUFFER_VIEW_DESC desc{};
        desc.BufferLocation = resource.GetGPUVirtualAddress();
        desc.SizeInBytes = static_cast<uint32_t>(bufferSize);
        device.GetDevice()->CreateConstantBufferView(&desc, descriptor.GetCPUHandle());

        descriptor_ = descriptor;
    }

    void ShaderResourceView::InitializeStructuredBuffer(
        const Device& device,
        const Resource& resource,
        uint32_t firstElement,
        uint32_t elementCount,
        uint32_t strideSize,
        const Descriptor& descriptor) {
        assert(device.IsEnabled());
        assert(resource.IsEnabled());
        assert(elementCount > 0);
        assert(strideSize > 0);
        assert(descriptor.IsEnabled());

        D3D12_RESOURCE_DESC resourceDesc = resource.GetResource()->GetDesc();

        D3D12_SHADER_RESOURCE_VIEW_DESC desc{};
        desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        desc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
        desc.Format = DXGI_FORMAT_UNKNOWN;
        desc.Buffer.FirstElement = firstElement;
        desc.Buffer.NumElements = elementCount;
        desc.Buffer.StructureByteStride = strideSize;
        desc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
        device.GetDevice()->CreateShaderResourceView(resource.GetResource().Get(), &desc, descriptor.GetCPUHandle());

        descriptor_ = descriptor;
        desc_ = desc;
    }

    void ShaderResourceView::InitializeByteAddressBuffer(
        const Device& device,
        const Resource& resource,
        uint32_t firstElement,
        uint32_t elementCount,
        const Descriptor& descriptor) {
        assert(device.IsEnabled());
        assert(resource.IsEnabled());
        assert(elementCount > 0);
        assert(descriptor.IsEnabled());

        D3D12_RESOURCE_DESC resourceDesc = resource.GetResource()->GetDesc();

        D3D12_SHADER_RESOURCE_VIEW_DESC desc{};
        desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        desc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
        desc.Format = DXGI_FORMAT_R32_TYPELESS;
        desc.Buffer.FirstElement = firstElement;
        desc.Buffer.NumElements = elementCount;
        desc.Buffer.StructureByteStride = 0;
        desc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_RAW;
        device.GetDevice()->CreateShaderResourceView(resource.GetResource().Get(), &desc, descriptor.GetCPUHandle());

        descriptor_ = descriptor;
        desc_ = desc;
    }

    void ShaderResourceView::InitializeTexture2D(
        const Device& device,
        const Resource& resource,
        const Descriptor& descriptor) {
        assert(device.IsEnabled());
        assert(resource.IsEnabled());
        assert(descriptor.IsEnabled());

        D3D12_RESOURCE_DESC resourceDesc = resource.GetResource()->GetDesc();

        D3D12_SHADER_RESOURCE_VIEW_DESC desc{};
        desc.Format = resourceDesc.Format;
        desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        desc.Texture2D.MipLevels = resourceDesc.MipLevels;
        device.GetDevice()->CreateShaderResourceView(resource.GetResource().Get(), &desc, descriptor.GetCPUHandle());

        descriptor_ = descriptor;
        desc_ = desc;
    }

    void UnorderedAccessView::InitializeStructuredBuffer(
        const Device& device,
        const Resource& resource,
        uint32_t firstElement,
        uint32_t elementCount,
        uint32_t strideSize,
        const Descriptor& descriptor) {
        assert(device.IsEnabled());
        assert(resource.IsEnabled());
        assert(elementCount > 0);
        assert(strideSize > 0);
        assert(descriptor.IsEnabled());

        D3D12_RESOURCE_DESC resourceDesc = resource.GetResource()->GetDesc();

        D3D12_UNORDERED_ACCESS_VIEW_DESC desc{};
        desc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
        desc.Format = DXGI_FORMAT_UNKNOWN;
        desc.Buffer.FirstElement = firstElement;
        desc.Buffer.NumElements = elementCount;
        desc.Buffer.StructureByteStride = strideSize;
        desc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;
        device.GetDevice()->CreateUnorderedAccessView(resource.GetResource().Get(), nullptr, &desc, descriptor.GetCPUHandle());

        descriptor_ = descriptor;
        desc_ = desc;
    }

    void UnorderedAccessView::InitializeByteAddressBuffer(
        const Device& device,
        const Resource& resource,
        uint32_t firstElement,
        uint32_t elementCount, const
        Descriptor& descriptor) {
        assert(device.IsEnabled());
        assert(resource.IsEnabled());
        assert(elementCount > 0);
        assert(descriptor.IsEnabled());

        D3D12_RESOURCE_DESC resourceDesc = resource.GetResource()->GetDesc();

        D3D12_UNORDERED_ACCESS_VIEW_DESC desc{};
        desc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
        desc.Format = DXGI_FORMAT_R32_TYPELESS;
        desc.Buffer.FirstElement = firstElement;
        desc.Buffer.NumElements = elementCount;
        desc.Buffer.StructureByteStride = 0;
        desc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_RAW;
        device.GetDevice()->CreateUnorderedAccessView(resource.GetResource().Get(), nullptr, &desc, descriptor.GetCPUHandle());

        descriptor_ = descriptor;
        desc_ = desc;
    }

    void UnorderedAccessView::InitializeTexture2D(
        const Device& device,
        const Resource& resource, 
        const Descriptor& descriptor) {
        assert(device.IsEnabled());
        assert(resource.IsEnabled());
        assert(descriptor.IsEnabled());

        D3D12_RESOURCE_DESC resourceDesc = resource.GetResource()->GetDesc();

        D3D12_UNORDERED_ACCESS_VIEW_DESC desc{};
        desc.Format = resourceDesc.Format;
        desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
        device.GetDevice()->CreateUnorderedAccessView(resource.GetResource().Get(), nullptr, &desc, descriptor.GetCPUHandle());

        descriptor_ = descriptor;
        desc_ = desc;
    }

    void RenderTargetView::Initialize(
        const Device& device,
        const Resource& resource,
        const Descriptor& descriptor,
        DXGI_FORMAT format) {
        assert(device.IsEnabled());
        assert(resource.IsEnabled());
        assert(descriptor.IsEnabled());

        D3D12_RENDER_TARGET_VIEW_DESC desc{};
        if (format == DXGI_FORMAT_UNKNOWN) {
            D3D12_RESOURCE_DESC resourceDesc = resource.GetResource()->GetDesc();
            desc.Format = resourceDesc.Format;
        }
        else {
            desc.Format = format;
        }
        desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
        device.GetDevice()->CreateRenderTargetView(resource.GetResource().Get(), &desc, descriptor.GetCPUHandle());

        descriptor_ = descriptor;
        desc_ = desc;
    }

    void DepthStencilView::Initialize(
        const Device& device,
        const Resource& resource,
        const Descriptor& descriptor) {
        assert(device.IsEnabled());
        assert(resource.IsEnabled());
        assert(descriptor.IsEnabled());

        D3D12_RESOURCE_DESC resourceDesc = resource.GetResource()->GetDesc();

        D3D12_DEPTH_STENCIL_VIEW_DESC desc{};
        desc.Format = resourceDesc.Format;
        desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
        device.GetDevice()->CreateDepthStencilView(resource.GetResource().Get(), &desc, descriptor.GetCPUHandle());

        descriptor_ = descriptor;
        desc_ = desc;
    }

}