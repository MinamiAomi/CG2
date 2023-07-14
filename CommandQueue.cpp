#include "CommandQueue.h"

#include "Device.h"

#include <cassert>

namespace CG::DX12 {

    Fence::Fence() :
        event_(nullptr),
        value_(0),
        waitValue_(0) {
    }

    DX12::Fence::~Fence() {
        if (event_) {
            CloseHandle(event_);
        }
    }

    void Fence::Initialize(const Device& device) {
        assert(device.IsEnabled());

        if (FAILED(device.GetDevice()->CreateFence(
            value_,
            D3D12_FENCE_FLAG_NONE,
            IID_PPV_ARGS(fence_.ReleaseAndGetAddressOf())))) {
            assert(false);
        }
        ++value_;
        event_ = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        assert(event_);
    }

    void Fence::Signal(const CommandQueue& commandQueue) {
        waitValue_ = value_;
        if (FAILED(commandQueue.GetComPtr()->Signal(fence_.Get(), waitValue_))) {
            assert(false);
        }
        ++value_;
    }

    void DX12::Fence::Signal(const CommandQueue& commandQueue, uint64 value) {
        if (FAILED(commandQueue.GetComPtr()->Signal(fence_.Get(), value))) {
            assert(false);
        }
    }

    void Fence::Wait() {
        if (fence_->GetCompletedValue() < waitValue_) {
            if (FAILED(fence_->SetEventOnCompletion(waitValue_, event_))) {
                assert(false);
            }
            WaitForSingleObject(event_, INFINITE);
        }
    }

    void DX12::Fence::Wait(uint64 value) {
        if (fence_->GetCompletedValue() < value) {
            if (FAILED(fence_->SetEventOnCompletion(value, event_))) {
                assert(false);
            }
            WaitForSingleObject(event_, INFINITE);
        }
    }

    void DX12::Fence::Wait(const CommandQueue& commandQueue) {
        commandQueue.GetComPtr()->Wait(fence_.Get(), waitValue_);
    }

    bool DX12::Fence::CheckSignal() const {
        return !(fence_->GetCompletedValue() < waitValue_);
    }

    void CommandQueue::Initialize(Microsoft::WRL::ComPtr<ID3D12Device> device, D3D12_COMMAND_LIST_TYPE type) {
        assert(device);
        D3D12_COMMAND_QUEUE_DESC desc{};
        desc.Type = type_ = type;
        if (FAILED(device->CreateCommandQueue(&desc, IID_PPV_ARGS(commandQueue_.ReleaseAndGetAddressOf())))) {
            assert(false);
        }
    }

}
