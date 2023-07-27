#include "GraphicsEngine.h"

#include "Window.h"

namespace CG {



    void GraphicsEngine::Initialize(const Window* window) {
        window_ = window;
        
        device_.Initialize();
        commandQueue_.Initialize(device_, D3D12_COMMAND_LIST_TYPE_DIRECT);
        fence_.Initialize(device_);

        rtvDescriptorHeap_.Initialize(device_, kRTVDescriptorHeapSize, D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        dsvDescriptorHeap_.Initialize(device_, kDSVDescriptorHeapSize, D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
        srvDescriptorHeap_.Initialize(device_, kSRVDescriptorHeapSize, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

        swapChain_.Initialize(window_->GetHWND(), device_, commandQueue_, rtvDescriptorHeap_, window->GetClientWidth(), window->GetClientHeight());
        viewport_ = DX12::Viewport(static_cast<float>(window->GetClientWidth()), static_cast<float>(window->GetClientHeight()));
        scissorRect_ = DX12::ScissorRect(viewport_);
    }

    DX12::CommandList& GraphicsEngine::PreDraw() {
        auto& cmdList = commandLists_[swapChain_.GetCurrentBackBufferIndex()];
        cmdList.Reset();


        return cmdList;
    }

}