#pragma once

#include <d3d12.h>
#include <wrl/client.h>

#include <atomic>
#include <cstdint>
#include <condition_variable>

#include "CommandAllocatorPool.h"

class CommandQueue {
    friend class CommandlistManager;
public:
    CommandQueue(D3D12_COMMAND_LIST_TYPE type);
    ~CommandQueue();

    void Create();
    void Destroy();

    uint64_t IncrementFence();
    bool IsFenceComplete(uint64_t fenceValue);
    
};