#pragma once

#include <d3d12.h>
#include <wrl/client.h>

#include <cstdint>

namespace CG::DX12 {
    using namespace Microsoft::WRL;

    class Device;
    class CommandQueue;

    class Fence {
    public:
        ~Fence();

        void Initialize(const Device& device);
        void Signal(const CommandQueue& commandQueue);
        void Signal(const CommandQueue& commandQueue, uint64_t value);
        void Wait() const;
        void Wait(uint64_t value) const;
        bool CheckSignal() const;

        bool IsEnabled() const { return fence_; }
        ComPtr<ID3D12Fence> GetFence() const { return fence_; }
        HANDLE GetEvent() const { return event_; }
        uint64_t GetValue() const { return value_; }
        uint64_t GetWaitValue() const { return waitValue_; }

    private:
        ComPtr<ID3D12Fence> fence_;
        HANDLE event_{ nullptr };
        uint64_t value_{ 0 };
        uint64_t waitValue_{ 0 };
    };

}