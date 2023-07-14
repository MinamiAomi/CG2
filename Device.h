#pragma once

#include <d3d12.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>
#include <combaseapi.h>
#include <wrl/client.h>

namespace CG::DX12 {
    using namespace Microsoft::WRL;

    class CommandQueue;

    class Device {
    public:
        void Initialize();
        void ResizeSwapChain(float width, float height);

        bool IsEnabled() const { return device_; }

        ComPtr<ID3D12Device> GetDevice()const { return device_; }

    private:
        ComPtr<IDXGIFactory7> dxgiFactory_;
        

        ComPtr<ID3D12Device> device_;
        ComPtr<ID3D12CommandQueue> graphicsQueue_;
        ComPtr<ID3D12CommandQueue> computeQueue_;
        ComPtr<ID3D12CommandQueue> copyQueue_;

        ComPtr<IDXGISwapChain4> swapChain_;

    };

}
