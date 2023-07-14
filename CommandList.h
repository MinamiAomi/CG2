#pragma once

#include <d3d12.h>
#include <wrl.h>

#include <cstdint>

namespace CG::DX12 {
    using namespace Microsoft::WRL;

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
        Microsoft::WRL::ComPtr<ID3D12CommandQueue> GetCommandQueue() const { return commandQueue_; }
        Microsoft::WRL::ComPtr<ID3D12Fence> GetFence() const { return fence_; }
        HANDLE GetFenceEvent() const { return fenceEvent_; }
        uint64_t GetFenceValue() const { return fenceValue_; }

    private:
        Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue_;
        Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator_;
        Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_;
        Microsoft::WRL::ComPtr<ID3D12Fence> fence_;
        HANDLE fenceEvent_;
        uint64_t fenceValue_;
    };

}