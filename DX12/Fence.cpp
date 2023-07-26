#include "Fence.h"

#include <cassert>

#include "Device.h"
#include "CommandQueue.h"

namespace CG::DX12 {

    Fence::~Fence() {
        if (event_) {
            CloseHandle(event_);
            event_ = nullptr;
        }
    }

    void Fence::Initialize(const Device& device) {
        assert(device.IsEnabled());

        fence_.Reset();

        if (event_) {
            CloseHandle(event_);
            event_ = nullptr;
        }

        if (FAILED(device.GetDevice()->CreateFence(
            value_,
            D3D12_FENCE_FLAG_NONE,
            IID_PPV_ARGS(fence_.GetAddressOf())))) {
            assert(false);
        }
        waitValue_ = value_ = 0;
        ++value_;
        event_ = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        assert(event_);
    }
    void Fence::Signal(const CommandQueue& commandQueue) {
        Signal(commandQueue, value_++);
    }

    void Fence::Signal(const CommandQueue& commandQueue, uint64_t value) {
        assert(IsEnabled());
        assert(commandQueue.IsEnabled());
        waitValue_ = value;
        if (FAILED(commandQueue.GetCommandQueue()->Signal(fence_.Get(), waitValue_))) {
            assert(false);
        }
    }

    void Fence::Wait() const {
        Wait(waitValue_);
    }

    void Fence::Wait(uint64_t value) const {
        assert(IsEnabled());
        if (fence_->GetCompletedValue() < value) {
            if (FAILED(fence_->SetEventOnCompletion(value, event_))) {
                assert(false);
            }
            WaitForSingleObject(event_, INFINITE);
        }
    }

    bool Fence::CheckSignal() const {
        return !(fence_->GetCompletedValue() < waitValue_);
    }

}