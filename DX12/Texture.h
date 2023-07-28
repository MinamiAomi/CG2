#pragma once

#include <string>

#include "Resource.h"

namespace CG::DX12 {

    class Device;
    class CommandQueue;

    class Texture {
    public:
        void InitializeFromPNG(const Device& device, CommandQueue& commandQueue, const std::wstring& name);

        Resource& GetResource() { return resource_; }
        const Resource& GetResource() const { return resource_; }
        const D3D12_RESOURCE_DESC& GetDesc() { return desc_; }

    private:
        Resource resource_;
        D3D12_RESOURCE_DESC desc_;
    };

}