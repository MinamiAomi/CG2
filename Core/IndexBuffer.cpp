#include "IndexBuffer.h"

#include "Defines.h"

IndexBuffer::IndexBuffer(const std::wstring& name) :
    GPUBuffer(name),
    numIndices_(0),
    indexFormat_(DXGI_FORMAT_UNKNOWN),
    indexBufferView_({}) {
}

IndexBuffer::~IndexBuffer() {
}

void IndexBuffer::CreateViews(size_t numElements, size_t elementSize) {
    // 16bitか32bitでなければならない
    ASSERT(elementSize == 2 || elementSize == 4);

    numIndices_ = numElements;
    indexFormat_ = (elementSize == 2) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;

    indexBufferView_.BufferLocation = resource_->GetGPUVirtualAddress();
    indexBufferView_.SizeInBytes = static_cast<uint32_t>(numElements * elementSize);
    indexBufferView_.Format = indexFormat_;
}

D3D12_CPU_DESCRIPTOR_HANDLE IndexBuffer::GetShaderResourceView(const D3D12_SHADER_RESOURCE_VIEW_DESC* srvDesc) const {
    ASSERT(false);
    return D3D12_CPU_DESCRIPTOR_HANDLE();
}

D3D12_CPU_DESCRIPTOR_HANDLE IndexBuffer::GetUnorderedAccessView(const D3D12_UNORDERED_ACCESS_VIEW_DESC* srvDesc) const {
    ASSERT(false);
    return D3D12_CPU_DESCRIPTOR_HANDLE();
}
