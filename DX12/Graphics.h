#pragma once

#include <d3d12.h>
#include <wrl/client.h>

class Graphics {
public:
    static Graphics* GetInstance();

    void Initialize();
    void Shutdown();

    const Microsoft::WRL::ComPtr<ID3D12Device>& GetDevice() const { return device_; }

private:
    Microsoft::WRL::ComPtr<ID3D12Device> device_;

};