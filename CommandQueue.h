#pragma once

#include <d3d12.h>
#include <wrl/client.h>

#include <cstdint>

#include "TypeUtils.h"

namespace CG::DX12 {
    using namespace Microsoft::WRL;

    class Device;
    class CommandQueue;
    class CommandList;
    class Fence;

    class Fence {
    public:
        Fence();
        ~Fence();
        void Initialize(const Device& device);
        void Signal(const CommandQueue& commandQueue);
        void Signal(const CommandQueue& commandQueue, uint64 value);
        void Wait();
        void Wait(uint64 value);
        void Wait(const CommandQueue& commandQueue);
        bool CheckSignal() const;

        bool IsEnabled() const { return fence_; }

    private:
        ComPtr<ID3D12Fence> fence_;
        HANDLE event_;
        uint64 value_;
        uint64 waitValue_;
    };

    class CommandList {

    };

    class CommandQueue {
    public:
        void Initialize(ComPtr<ID3D12Device> device, D3D12_COMMAND_LIST_TYPE type);

        bool IsEnabled() const { return commandQueue_; }
        ComPtr<ID3D12CommandQueue> GetComPtr() const { return commandQueue_; }
        ID3D12CommandQueue* GetRawPtr() const { return commandQueue_.Get(); }
        D3D12_COMMAND_LIST_TYPE GetType() const { return type_; }

    private:
        ComPtr<ID3D12CommandQueue> commandQueue_;
        D3D12_COMMAND_LIST_TYPE type_;
    };

}