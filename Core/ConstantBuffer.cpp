#include "ConstantBuffer.h"

#include "Utility.h"
#include "Defines.h"
#include "Graphics.h"

ConstantBuffer::ConstantBuffer(const std::wstring& name) :
    GPUBuffer(name),
    sizeInBytes_(0) {
}

ConstantBuffer::~ConstantBuffer() {
}

void ConstantBuffer::CreateViews(size_t numElements, size_t elementSize) {
    auto& graphics = Graphics::Get();
    auto device = graphics.GetDevice();

    sizeInBytes_ = numElements * elementSize;

    D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc{};
    cbvDesc.BufferLocation = resource_->GetGPUVirtualAddress();
    cbvDesc.SizeInBytes = static_cast<uint32_t>(Utility::AlignUp(sizeInBytes_, 16));

    if (cbv_.IsNull()) {
        cbv_ = graphics.AllocateDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    }
    device->CreateConstantBufferView(&cbvDesc, cbv_.GetDescriptorHandle());
}

D3D12_CPU_DESCRIPTOR_HANDLE ConstantBuffer::GetShaderResourceView(const D3D12_SHADER_RESOURCE_VIEW_DESC* srvDesc) const {
    ASSERT(false);
    return D3D12_CPU_DESCRIPTOR_HANDLE();
}

D3D12_CPU_DESCRIPTOR_HANDLE ConstantBuffer::GetUnorderedAccessView(const D3D12_UNORDERED_ACCESS_VIEW_DESC* uavDesc) const {
    ASSERT(false);
    return D3D12_CPU_DESCRIPTOR_HANDLE();
}
