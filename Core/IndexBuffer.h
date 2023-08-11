#pragma once
#include "GPUBuffer.h"

#include <cstdint>

class IndexBuffer : public GPUBuffer {
public:
    IndexBuffer(const std::wstring& name = L"");

    virtual  ~IndexBuffer();

    virtual void CreateViews(size_t numElements, size_t elementSize) override;
   
    // 使用不可
    virtual D3D12_CPU_DESCRIPTOR_HANDLE GetShaderResourceView(const D3D12_SHADER_RESOURCE_VIEW_DESC* srvDesc = nullptr) const override;
    // 使用不可
    virtual D3D12_CPU_DESCRIPTOR_HANDLE GetUnorderedAccessView(const D3D12_UNORDERED_ACCESS_VIEW_DESC* uavDesc = nullptr) const override;


    const D3D12_INDEX_BUFFER_VIEW& GetVertexBufferView() const { return indexBufferView_; }
    size_t GetNumindices() const { return numIndices_; }
    DXGI_FORMAT GetIndexFormat() const { return indexFormat_; }
    
private:
    size_t numIndices_;
    DXGI_FORMAT indexFormat_;
    D3D12_INDEX_BUFFER_VIEW indexBufferView_;
};