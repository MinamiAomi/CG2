#pragma once

#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl/client.h>

namespace CG::DX12 {
    using namespace Microsoft::WRL;

    class Device {
    public:
        void Initialize();

        bool IsEnabled() const { return device_; }
        ComPtr<ID3D12Device> GetDevice() const { return device_; }
        ComPtr<IDXGIFactory7> GetFactory() const { return factory_; }
        ComPtr<IDXGIAdapter4> GetAdapter() const { return adapter_; }

    private:
        ComPtr<ID3D12Device> device_;
        ComPtr<IDXGIFactory7> factory_;
        ComPtr<IDXGIAdapter4> adapter_;
    };

}