#include "ByteAddressBuffer.h"

#include "Graphics.h"
#include "Utility.h"

ByteAddressBuffer::ByteAddressBuffer(const std::wstring& name) {
}

ByteAddressBuffer::ByteAddressBuffer(const D3D12_RESOURCE_DESC& resourceDesc, size_t numElements, size_t elementSize, const std::wstring& name) :
    GPUBuffer(resourceDesc, numElements, elementSize, name) {
}

void ByteAddressBuffer::CreateViews(size_t numElements, size_t elementSize) {
    auto& graphics = Graphics::Get();
    auto device = graphics.GetDevice();
    // 4バイトごとにアライメントされている必要がある
    bufferSize_ = Utility::AlignUp(numElements * elementSize, 4);

    uint32_t numElements = uint32_t(bufferSize_) / 4;

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
    srvDesc.Format = DXGI_FORMAT_R32_TYPELESS;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Buffer.NumElements = numElements;
    srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_RAW;

    if (srv_.IsNull()) {
        srv_ = graphics.AllocateDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    }
    device->CreateShaderResourceView(resource_.Get(), &srvDesc, srv_.GetDescriptorHandle());

    D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
    uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
    uavDesc.Format = DXGI_FORMAT_R32_TYPELESS;
    uavDesc.Buffer.NumElements = numElements;
    uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_RAW;

    if (uav_.IsNull()) {
        uav_ = graphics.AllocateDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    }
    device->CreateUnorderedAccessView(resource_.Get(), nullptr, &uavDesc, uav_.GetDescriptorHandle());
}
