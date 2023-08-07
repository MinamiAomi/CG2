#pragma once
#include "GPUResource.h"

#include <string>

class UploadBuffer : public GPUResource {
public:
    virtual ~UploadBuffer() { Destory(); }

    void Create(const std::wstring& name, size_t bufferSize);
    void* Map();
    void Unmap();

    size_t GetBufferSize() const { return bufferSize_; }

private:
    size_t bufferSize_;
};
