#include "StructuredBuffer.h"

#include "../Externals/DirectXTex/d3dx12.h"

#include "Graphics.h"

StructuredBuffer::StructuredBuffer(const std::wstring& name) :
    GPUBuffer(name),
    counterBuffer_(CD3DX12_RESOURCE_DESC::Buffer(4, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS), 1, 4, name + L" Counter"),
    numElements_(0),
    elementSize_(0) {
}

StructuredBuffer::StructuredBuffer(const D3D12_RESOURCE_DESC& resourceDesc, size_t numElements, size_t elementSize, const std::wstring& name) :
    GPUBuffer(resourceDesc, numElements, elementSize, name),
    counterBuffer_(CD3DX12_RESOURCE_DESC::Buffer(4, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS), 1, 4, name + L" Counter"),
    numElements_(numElements),
    elementSize_(elementSize) {
}

void StructuredBuffer::CreateViews(size_t numElements, size_t elementSize) {
    auto& graphics = Graphics::Get();
    auto device = graphics.GetDevice();

    numElements_ = numElements;
    elementSize_ = elementSize;

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
    srvDesc.Format = DXGI_FORMAT_UNKNOWN;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Buffer.NumElements = static_cast<uint32_t>(numElements_);
    srvDesc.Buffer.StructureByteStride = static_cast<uint32_t>(elementSize_);
    srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_RAW;

    if (srv_.IsNull()) {
        srv_ = graphics.AllocateDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    }
    device->CreateShaderResourceView(resource_.Get(), &srvDesc, srv_.GetDescriptorHandle());

    D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
    uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
    uavDesc.Format = DXGI_FORMAT_UNKNOWN;
    uavDesc.Buffer.NumElements = static_cast<uint32_t>(numElements_);
    uavDesc.Buffer.StructureByteStride = static_cast<uint32_t>(elementSize_);
    uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_RAW;

    if (uav_.IsNull()) {
        uav_ = graphics.AllocateDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    }
    device->CreateUnorderedAccessView(resource_.Get(), counterBuffer_.GetResource().Get(), &uavDesc, uav_.GetDescriptorHandle());
}
