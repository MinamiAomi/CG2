#pragma once

#include <d3d12.h>
#include <wrl/client.h>

#include <cstdint>
#include <initializer_list>

namespace CG::DX12 {
    using namespace Microsoft::WRL;

    class Device;
    class CommandList;
    class Fence;

    class CommandQueue {
    public:
        void Initialize(const Device& device, D3D12_COMMAND_LIST_TYPE type);
        void ExcuteCommandList(const CommandList& commandList);
        void ExcuteCommandLists(std::initializer_list<const CommandList*> list);
        void Wait(const Fence& fence) const;

        bool IsEnabled() const { return commandQueue_; }
        ComPtr<ID3D12CommandQueue> GetCommandQueue() const { return commandQueue_; }
        D3D12_COMMAND_LIST_TYPE GetType() const { return type_; }

    private:
        ComPtr<ID3D12CommandQueue> commandQueue_;
        D3D12_COMMAND_LIST_TYPE type_{ D3D12_COMMAND_LIST_TYPE_DIRECT };
    };

}