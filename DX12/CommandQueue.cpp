#include "CommandQueue.h"

#include <cassert>
#include <vector>

#include "Device.h"
#include "CommandList.h"
#include "Fence.h"

namespace CG::DX12 {

    void CommandQueue::Initialize(const Device& device, D3D12_COMMAND_LIST_TYPE type) {
        assert(device.IsEnabled());
        D3D12_COMMAND_QUEUE_DESC desc{};
        desc.Type = type_ = type;
        if (FAILED(device.GetDevice()->CreateCommandQueue(&desc, IID_PPV_ARGS(commandQueue_.ReleaseAndGetAddressOf())))) {
            assert(false);
        }
    }

    void CommandQueue::ExcuteCommandList(const CommandList& commandList) {
        assert(commandList.IsEnabled() && !commandList.IsRecording());
        ID3D12CommandList* ppCmdList[]{ commandList.GetCommandList().Get() };
        commandQueue_->ExecuteCommandLists(1, ppCmdList);
    }

    void CommandQueue::ExcuteCommandLists(std::initializer_list<const CommandList*> list) {
        std::vector<ID3D12CommandList*> ppCmdList(list.size());
        for (auto dest = ppCmdList.begin(); auto src : list) {
            assert(src->IsEnabled() && !src->IsRecording());
            *dest = src->GetCommandList().Get();
            ++dest;
        }
        commandQueue_->ExecuteCommandLists(static_cast<uint32_t>(ppCmdList.size()), ppCmdList.data());
    }

    void CommandQueue::Wait(const Fence& fence) const {
        if (FAILED(commandQueue_->Wait(fence.GetFence().Get(), fence.GetWaitValue()))) {
            assert(false);
        }
    }

}
