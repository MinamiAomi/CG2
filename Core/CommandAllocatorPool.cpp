#include "CommandAllocatorPool.h"

#include "Defines.h"
#include "Graphics.h"

CommandAllocatorPool::CommandAllocatorPool(D3D12_COMMAND_LIST_TYPE type) :
    type_(type) {
}

CommandAllocatorPool::~CommandAllocatorPool() {
}

Microsoft::WRL::ComPtr<ID3D12CommandAllocator> CommandAllocatorPool::RequestAllocator(uint64_t completedFenceValue) {
    std::lock_guard<std::mutex> lock(allocationMutex_);

    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> allocator;

    if (!readyAllocators_.empty()) {
        auto& allocatorPair = readyAllocators_.front();
        if (allocatorPair.first <= completedFenceValue) {
            allocator = allocatorPair.second;
            readyAllocators_.pop();
        }
    }

    if (!allocator) {
        auto device = Graphics::Get().GetDevice();
        ASSERT_IF_FAILED(device->CreateCommandAllocator(type_, IID_PPV_ARGS(allocator.GetAddressOf())));
        commandAllocatorPool_.emplace_back(allocator);
    }
    
    return allocator;
}

void CommandAllocatorPool::DiscardAllocator(uint64_t fenceValue, Microsoft::WRL::ComPtr<ID3D12CommandAllocator> allocator) {
    std::lock_guard<std::mutex> lock(allocationMutex_);
    readyAllocators_.push(std::make_pair(fenceValue, allocator));
}
