#include "SwapChain.h"

#include <cassert>

#include "Device.h"
#include "CommandQueue.h"

namespace CG::DX12 {

    void SwapChain::Initialize(
        HWND hWnd,
        const Device& device,
        const CommandQueue& commandQueue,
        DescriptorHeap& descriptorHeap,
        uint32_t width,
        uint32_t height) {
        assert(hWnd);
        assert(device.IsEnabled());
        assert(commandQueue.IsEnabled());
        assert(descriptorHeap.IsEnabled());
        assert(width > 0);
        assert(height > 0);

        // スワップチェーンの設定
        DXGI_SWAP_CHAIN_DESC1 desc{};
        desc.Width = width; // 画面幅
        desc.Height = height; // 画面高
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // 色の形式
        desc.SampleDesc.Count = 1; // マルチサンプル市内
        desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // 描画ターゲットとして利用する
        desc.BufferCount = kBackBufferCount;
        desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // モニタに移したら、中身を破棄
        // スワップチェーンを生成
        if (FAILED(device.GetFactory()->CreateSwapChainForHwnd(
            commandQueue.GetCommandQueue().Get(),
            hWnd,
            &desc,
            nullptr,
            nullptr,
            reinterpret_cast<IDXGISwapChain1**>(swapChain_.ReleaseAndGetAddressOf())))) {
            assert(false);
        }

        for (uint32_t i = 0; i < kBackBufferCount; ++i) {
            ComPtr<ID3D12Resource> resource;
            swapChain_->GetBuffer(i, IID_PPV_ARGS(resource.GetAddressOf()));
            resources_[i].InitializeForResource(resource);

            renderTargetViews_[i].Initialize(device, resources_[i], descriptorHeap.Allocate(), DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);
        }
        currentBackBufferIndex_ = swapChain_->GetCurrentBackBufferIndex();
    }

    void SwapChain::Present(uint32_t syncInterval) {
        assert(IsEnabled());
        swapChain_->Present(syncInterval, 0);
        currentBackBufferIndex_ = swapChain_->GetCurrentBackBufferIndex();
    }

}