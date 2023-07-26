#pragma once

#include <dxgi1_6.h>
#include <wrl/client.h>

#include <cstdint>

#include "DescriptorHeap.h"
#include "Resource.h"
#include "Views.h"

namespace CG::DX12 {
    using namespace Microsoft::WRL;

    class Device;
    class CommandQueue;

    class SwapChain {
    public:
        static const uint32_t kBackBufferCount = 2;

        void Initialize(
            HWND hWnd,
            const Device& device,
            const CommandQueue& commandQueue,
            DescriptorHeap& descriptorHeap,
            uint32_t width,
            uint32_t height);
        void Present(uint32_t syncInterval);

        bool IsEnabled() const { return swapChain_; }
        ComPtr<IDXGISwapChain4> GetSwapChain() const { return swapChain_; }
        Resource& GetCurrentResource() { return resources_[currentBackBufferIndex_]; }
        const Resource& GetCurrentResource() const { return resources_[currentBackBufferIndex_]; }
        RenderTargetView& GetCorrentRenderTargetView() { return renderTargetViews_[currentBackBufferIndex_]; }
        const RenderTargetView& GetCurrentRenderTargetView() const { return renderTargetViews_[currentBackBufferIndex_]; }
        uint32_t GetCurrentBackBufferIndex() { return currentBackBufferIndex_; }

    private:
        ComPtr<IDXGISwapChain4> swapChain_;
        Resource resources_[kBackBufferCount];
        RenderTargetView renderTargetViews_[kBackBufferCount];
        uint32_t currentBackBufferIndex_{ 0 };
    };

}