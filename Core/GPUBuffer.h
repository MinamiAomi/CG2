#pragma once
#include "GPUResource.h"

class GPUBuffer : public GPUResource {
public:
    GPUBuffer(const std::wstring& name = L"");
    GPUBuffer(const D3D12_RESOURCE_DESC& resourceDesc, size_t numElements, size_t elementSize, const std::wstring& name = L"");

    virtual ~GPUBuffer();

    virtual void CreateViews(size_t numElements, size_t elementSize) = 0;
};