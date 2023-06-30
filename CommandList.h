#pragma once

#include <d3d12.h>

#include <cstdint>

class CommandList {
public:
    CommandList();
    ~CommandList();

    void Initialize(ID3D12Device* device);
    void Finalize();

    void ExcuteCommand();
    void WaitForGPU();
    void Reset();

    bool IsEnabled() const { return commandQueue_; }
    ID3D12GraphicsCommandList* Get() const { return commandList_; }
    ID3D12CommandQueue* GetCommandQueue() const { return commandQueue_; }

private:
    ID3D12CommandQueue* commandQueue_;
    ID3D12CommandAllocator* commandAllocator_;
    ID3D12GraphicsCommandList* commandList_;
    ID3D12Fence* fence_;
    HANDLE fenceEvent_;
    uint64_t fenceValue_;
};