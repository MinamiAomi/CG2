#include "GraphicsEngine.h"

#include "Window.h"

namespace CG {
    GraphicsEngine* GraphicsEngine::GetInstance() {
        static GraphicsEngine instance;
        return &instance;
    }
    void GraphicsEngine::Initialize(const Window* window) {
        window_ = window;

        device_.Initialize();
        commandQueue_.Initialize(device_, D3D12_COMMAND_LIST_TYPE_DIRECT);
        commandList_.Initialize(device_, commandQueue_);
        fence_.Initialize(device_);
        commandList_.Reset();

        rtvDescriptorHeap_.Initialize(device_, kRTVDescriptorHeapSize, D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        dsvDescriptorHeap_.Initialize(device_, kDSVDescriptorHeapSize, D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
        srvDescriptorHeap_.Initialize(device_, kSRVDescriptorHeapSize, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

        swapChain_.Initialize(window_->GetHWND(), device_, commandQueue_, rtvDescriptorHeap_, window->GetClientWidth(), window->GetClientHeight());
        depthStencilResource.Initialize(device_, dsvDescriptorHeap_, window->GetClientWidth(), window->GetClientHeight());
        viewport_ = DX12::Viewport(static_cast<float>(window->GetClientWidth()), static_cast<float>(window->GetClientHeight()));
        scissorRect_ = DX12::ScissorRect(viewport_);

        shaderCompiler_.Initialize();
    }

    void GraphicsEngine::PreDraw() {
        auto cmdList = commandList_.GetCommandList();

        auto barrier = swapChain_.GetCurrentResource().TransitionBarrier(DX12::Resource::State::RenderTarget);
        cmdList->ResourceBarrier(1, &barrier);

        auto rtv = swapChain_.GetCorrentRenderTargetView().GetDescriptor().GetCPUHandle();
        auto dsv = depthStencilResource.GetCPUHandle();
        cmdList->OMSetRenderTargets(1, &rtv, 0, &dsv);
        float clearColor[4] = { 0.1f,0.3f,0.5f,1.0f };
        cmdList->ClearRenderTargetView(rtv, clearColor, 0, nullptr);
        cmdList->ClearDepthStencilView(dsv, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

        cmdList->RSSetViewports(1, &viewport_);
        cmdList->RSSetScissorRects(1, &scissorRect_);
    }

    void GraphicsEngine::PostDraw() {
        auto barrier = swapChain_.GetCurrentResource().TransitionBarrier(DX12::Resource::State::Present);
        commandList_.GetCommandList()->ResourceBarrier(1, &barrier);

        commandList_.Close();
        commandQueue_.ExcuteCommandList(commandList_);
        swapChain_.Present(1);
        fence_.Signal(commandQueue_);
        fence_.Wait();
        commandList_.Reset();
    }

    void GraphicsEngine::SetDescriptorHeap() {
        ID3D12DescriptorHeap* ppHeaps[] = { srvDescriptorHeap_.GetDescriptorHeap().Get() };
        commandList_.GetCommandList()->SetDescriptorHeaps(1, ppHeaps);
    }

}