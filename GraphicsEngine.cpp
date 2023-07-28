#include "GraphicsEngine.h"

#include "Window.h"

namespace CG {



    void GraphicsEngine::Initialize(const Window* window) {
        window_ = window;

        device_.Initialize();
        commandQueue_.Initialize(device_, D3D12_COMMAND_LIST_TYPE_DIRECT);
        for (auto& commandList : commandLists_) {
            commandList.Initialize(device_, commandQueue_);
        }
        fence_.Initialize(device_);

        rtvDescriptorHeap_.Initialize(device_, kRTVDescriptorHeapSize, D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        dsvDescriptorHeap_.Initialize(device_, kDSVDescriptorHeapSize, D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
        srvDescriptorHeap_.Initialize(device_, kSRVDescriptorHeapSize, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

        swapChain_.Initialize(window_->GetHWND(), device_, commandQueue_, rtvDescriptorHeap_, window->GetClientWidth(), window->GetClientHeight());
        depthStencilResource.Initialize(device_, dsvDescriptorHeap_, window->GetClientWidth(), window->GetClientHeight());
        viewport_ = DX12::Viewport(static_cast<float>(window->GetClientWidth()), static_cast<float>(window->GetClientHeight()));
        scissorRect_ = DX12::ScissorRect(viewport_);
    }

    DX12::CommandList& GraphicsEngine::PreDraw() {
        auto& commandList = commandLists_[swapChain_.GetCurrentBackBufferIndex()];
        commandList.Reset();
        auto cmdList = commandList.GetCommandList();

        auto barrier = swapChain_.GetCurrentResource().TransitionBarrier(DX12::Resource::State::RenderTarget);
        cmdList->ResourceBarrier(1, &barrier);

        auto rtv = swapChain_.GetCorrentRenderTargetView().GetDescriptor().GetCPUHandle();
        auto dsv = depthStencilResource.GetCPUHandle();
        cmdList->OMSetRenderTargets(1, &rtv, 0, &dsv);
        float clearColor[4] = { 0.2f,0.2f,0.2f,1.0f };
        cmdList->ClearRenderTargetView(rtv, clearColor, 0, nullptr);
        cmdList->ClearDepthStencilView(dsv, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

        cmdList->RSSetViewports(1, &viewport_);
        cmdList->RSSetScissorRects(1, &scissorRect_);

        return commandList;
    }

    void GraphicsEngine::PostDraw(DX12::CommandList& commandList) {
        auto barrier = swapChain_.GetCurrentResource().TransitionBarrier(DX12::Resource::State::Present);
        commandList.GetCommandList()->ResourceBarrier(1, &barrier);

        commandList.Close();
        fence_.Wait();
        commandQueue_.ExcuteCommandList(commandList);
        swapChain_.Present(1);
        fence_.Signal(commandQueue_);
    }

}