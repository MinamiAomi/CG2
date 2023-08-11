#pragma once
#include "GPUBuffer.h"

#include <cstdint>

class VertexBuffer : public GPUBuffer {
public:
    VertexBuffer(const std::wstring& name = L"");

    virtual  ~VertexBuffer();

    virtual void CreateViews(size_t numElements, size_t elementSize) override;

    // 使用不可
    virtual D3D12_CPU_DESCRIPTOR_HANDLE GetShaderResourceView(const D3D12_SHADER_RESOURCE_VIEW_DESC* srvDesc = nullptr) const override;
    // 使用不可
    virtual D3D12_CPU_DESCRIPTOR_HANDLE GetUnorderedAccessView(const D3D12_UNORDERED_ACCESS_VIEW_DESC* uavDesc = nullptr) const override;

    const D3D12_VERTEX_BUFFER_VIEW& GetVertexBufferView() const { return vertexBufferView_; }
    size_t GetNumVertices() const { return numVertices_; }
    size_t GetVertexStride() const { return vertexStride_; }

private:
    size_t numVertices_;
    size_t vertexStride_;
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;
};