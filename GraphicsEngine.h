#pragma once

#include "DX12/DX12.h"
#include "Window.h"

namespace CG {

    class Window;

    class GraphicsEngine {
    public:
        static GraphicsEngine* GetInstance();

        void Initialize(const Window* targetWindow);
        void PreDraw();
        void PostDraw();
        
        const Window* GetWindow() const { return window_; }
        DX12::Device& GetDevice() { return device_; }
        DX12::CommandQueue& GetCommandQueue() { return commandQueue_; }
        DX12::CommandList& GetCommandList() { return commandList_; }
        DX12::Fence& GetFence() { return fence_; }
        DX12::DescriptorHeap& GetRTVDescriptorHeap() { return rtvDescriptorHeap_; }
        DX12::DescriptorHeap& GetDSVDescriptorHeap() { return dsvDescriptorHeap_; }
        DX12::DescriptorHeap& GetSRVDescriptorHeap() { return srvDescriptorHeap_; }
        DX12::SwapChain& GetSwapChain() { return swapChain_; }

    private:
        GraphicsEngine() = default;
        GraphicsEngine(const GraphicsEngine&) = delete;
        const GraphicsEngine& operator=(const GraphicsEngine&) = delete;

        static const uint32_t kRTVDescriptorHeapSize = 16;
        static const uint32_t kDSVDescriptorHeapSize = 8;
        static const uint32_t kSRVDescriptorHeapSize = 1024;

        const Window* window_{ nullptr };

        DX12::Device device_;

        DX12::CommandQueue commandQueue_;
        DX12::CommandList commandList_;
        DX12::Fence fence_;

        DX12::DescriptorHeap rtvDescriptorHeap_;
        DX12::DescriptorHeap dsvDescriptorHeap_;
        DX12::DescriptorHeap srvDescriptorHeap_;

        DX12::SwapChain swapChain_;
        DX12::DepthStencilResource depthStencilResource;
        DX12::Viewport viewport_;
        DX12::ScissorRect scissorRect_;
    };

}