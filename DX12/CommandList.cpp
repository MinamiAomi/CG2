#include "CommandList.h"

#include <cassert>

#include "Device.h"
#include "CommandQueue.h"
#include "Views.h"

namespace CG::DX12 {

    void CommandList::Initialize(const Device& device, const CommandQueue& commandQueue) {
        assert(device.IsEnabled());
        assert(commandQueue.IsEnabled());

        commandAllocator_.Reset();
        commandList_.Reset();

        type_ = commandQueue.GetType();
        if (FAILED(device.GetDevice()->CreateCommandAllocator(
            type_,
            IID_PPV_ARGS(commandAllocator_.GetAddressOf())))) {
            assert(false);
        }
        if (FAILED(device.GetDevice()->CreateCommandList(
            0, type_,
            commandAllocator_.Get(),
            nullptr,
            IID_PPV_ARGS(commandList_.GetAddressOf())))) {
            assert(false);
        }
        isRecording_ = true;
        Close();
    }

    void CommandList::Close() {
        assert(IsEnabled());
        assert(isRecording_);
        if (FAILED(commandList_->Close())) {
            assert(false);
        }
        isRecording_ = false;
    }

    void CommandList::Reset() {
        assert(!isRecording_);
        // 次フレーム用のコマンドリストを準備
        if (FAILED(commandAllocator_->Reset())) {
            assert(false);
        }
        if (FAILED(commandList_->Reset(commandAllocator_.Get(), nullptr))) {
            assert(false);
        }
        isRecording_ = true;
    }

}