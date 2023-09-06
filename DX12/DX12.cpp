#include "DX12.h"

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"dxguid.lib")

namespace CG::DX12 {

    void DynamicBuffer::Initialize(const Device& device, size_t bufferSize, bool allowUnorderedAccess, CG::DX12::Resource::State initialState) {
        bufferSize_ = bufferSize;
        resource_.InitializeForBuffer(
            device,
            bufferSize_,
            initialState,
            CG::DX12::Resource::HeapType::Upload,
            allowUnorderedAccess ? CG::DX12::Resource::Flag::AllowUnorderedAccess : CG::DX12::Resource::Flag::None);
        dataBegin_ = resource_.Map();
        memset(dataBegin_, 0, bufferSize_);
    }

    void DepthStencilResource::Initialize(const Device& device, DescriptorHeap& descriptorHeap, uint32_t width, uint32_t height) {
        resource_.InitializeForTexture2D(device, width, height, 2, 1, DXGI_FORMAT_D24_UNORM_S8_UINT, Resource::State::DepthWrite, Resource::HeapType::Default, Resource::Flag::AllowDepthStencil);
        view_.Initialize(device, resource_, descriptorHeap.Allocate());
    }

}