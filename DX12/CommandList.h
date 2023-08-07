#pragma once

#include <d3d12.h>
#include <wrl.h>

#include <cstdint>
#include <initializer_list>

namespace CG::DX12 {
    using namespace Microsoft::WRL;

    class Device;
    class CommandQueue;
    class VertexBufferView;
    class IndexBufferView;

    class CommandList {
    public:
        void Initialize(const Device& device, const CommandQueue& commandQueue);
        void Close();
        void Reset();

        bool IsEnabled() const { return commandList_; }
        bool IsRecording() const { return isRecording_; }
        ComPtr<ID3D12GraphicsCommandList> GetCommandList() const { return commandList_; }
        D3D12_COMMAND_LIST_TYPE GetType() const { return type_; }

    private:
        ComPtr<ID3D12CommandAllocator> commandAllocator_;
        ComPtr<ID3D12GraphicsCommandList> commandList_;
        D3D12_COMMAND_LIST_TYPE type_{ D3D12_COMMAND_LIST_TYPE_DIRECT };
        bool isRecording_{ false };
    };

}