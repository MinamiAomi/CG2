#include "VertexBuffer.h"

#include "Defines.h"

VertexBuffer::VertexBuffer(const std::wstring& name) :
    GPUBuffer(name),
    numVertices_(0),
    vertexStride_(0),
    vertexBufferView_({}) {
}

VertexBuffer::~VertexBuffer() {
}

void VertexBuffer::CreateViews(size_t numElements, size_t elementSize) {
    numVertices_ = numElements;
    vertexStride_ = elementSize;

    vertexBufferView_.BufferLocation = resource_->GetGPUVirtualAddress();
    vertexBufferView_.SizeInBytes = static_cast<uint32_t>(numVertices_ * vertexStride_);
    vertexBufferView_.StrideInBytes = static_cast<uint32_t>(vertexStride_);
}

D3D12_CPU_DESCRIPTOR_HANDLE VertexBuffer::GetShaderResourceView(const D3D12_SHADER_RESOURCE_VIEW_DESC* srvDesc) const {
    ASSERT(false);
    return D3D12_CPU_DESCRIPTOR_HANDLE();
}

D3D12_CPU_DESCRIPTOR_HANDLE VertexBuffer::GetUnorderedAccessView(const D3D12_UNORDERED_ACCESS_VIEW_DESC* srvDesc) const {
    ASSERT(false);
    return D3D12_CPU_DESCRIPTOR_HANDLE();
}
