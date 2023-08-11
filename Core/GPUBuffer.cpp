#include "GPUBuffer.h"

#include "Defines.h"

GPUBuffer::GPUBuffer(const std::wstring& name) :
    GPUResource(name) {
}

GPUBuffer::GPUBuffer(const D3D12_RESOURCE_DESC& resourceDesc, size_t numElements, size_t elementSize, const std::wstring& name) :
    GPUResource(resourceDesc, nullptr, name) {
    CreateViews(numElements, elementSize);
}

GPUBuffer::~GPUBuffer() {
}

void GPUBuffer::CreateViews(size_t numElements, size_t elementSize) {
    ASSERT(false);
}
