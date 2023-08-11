#pragma once

#include <d3d12.h>
#include <wrl/client.h>

#include <cstdint>
#include <queue>
#include <mutex>
#include <vector>

class CommandAllocatorPool {
public:
    CommandAllocatorPool(D3D12_COMMAND_LIST_TYPE type);
    ~CommandAllocatorPool();

    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> RequestAllocator(uint64_t completedFenceValue);
    void DiscardAllocator(uint64_t fenceValue, Microsoft::WRL::ComPtr<ID3D12CommandAllocator> allocator);

private:
    const D3D12_COMMAND_LIST_TYPE type_;

    std::vector<Microsoft::WRL::ComPtr<ID3D12CommandAllocator>> commandAllocatorPool_;
    std::queue<std::pair<uint64_t, Microsoft::WRL::ComPtr<ID3D12CommandAllocator>>> readyAllocators_;
    std::mutex allocationMutex_;
};