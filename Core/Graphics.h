#pragma once

#include <d3d12.h>
#include <wrl/client.h>

#include <cstdint>
#include <memory>
#include <string>

#include "DescriptorAllocation.h"

class CommandQueue;
class DescriptorAllocator;

class Graphics {
public:
    static void Create();
    static void Destroy();

    static Graphics& Get();

    static uint64_t GetFrameCount() { return frameCount_; }

    Microsoft::WRL::ComPtr<ID3D12Device> GetDevice() const { return device_; }

    DescriptorAllocation AllocateDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptors = 1);



private:
    static uint64_t frameCount_;
    
    Graphics() = default;
    Graphics(const Graphics&) = delete;
    const Graphics& operator=(const Graphics&) = delete;
    ~Graphics() = default;

    void Initialize();

    Microsoft::WRL::ComPtr<ID3D12Device> device_;

    std::unique_ptr<CommandQueue> directCommandQueue_;
    std::unique_ptr<CommandQueue> computeCommandQueue_;
    std::unique_ptr<CommandQueue> copyCommandQueue_;

    std::unique_ptr<DescriptorAllocator> descriptorAllocators_[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];

};