#pragma once

#include <d3d12.h>
#include <wrl.h>

#include <cstdint>

class CommandList {
public:
    CommandList();
    ~CommandList();

    void Initialize(Microsoft::WRL::ComPtr<ID3D12Device> device, Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue);

    void ExcuteCommand();
    void WaitForGPU();
    void Reset();

    bool IsEnabled() const { return commandQueue_; }
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> Get() const { return commandList_; }

private:
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue_;
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator_;
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_;
    Microsoft::WRL::ComPtr<ID3D12Fence> fence_;
    HANDLE fenceEvent_;
    uint64_t fenceValue_;
};