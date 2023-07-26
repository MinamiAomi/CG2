#pragma once

#include <d3d12.h>

#include <cstdint>
#include <memory>

namespace CG::DX12 {

    class DescriptorHeap;

    class Descriptor {
        friend class DescriptorHeap;
    public:
        bool IsEnabled() const { return static_cast<bool>(data_); }
        bool ShaderVisible() const { return data_->gpuHandle.ptr != 0; }
        D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle() const { return data_->cpuHandle; }
        D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle() const { return data_->gpuHandle; }

    private:
        struct Data {
            DescriptorHeap* descriptorHeap{ nullptr };
            D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle{};
            D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle{};
            uint32_t index{ 0 };

            ~Data();
        };
        std::shared_ptr<Data> data_;
    };

  /*  class Descriptor {
        friend class DescriptorHeap;
    public:
        ~Descriptor();



        bool IsEnabled() const { return descriptorHeap_; }
        bool ShaderVisible() const { return gpuHandle_.ptr != 0; }
        D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle() const { return cpuHandle_; }
        D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle() const { return gpuHandle_; }

    private:
        DescriptorHeap* descriptorHeap_{ nullptr };
        D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle_{};
        D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle_{};
        uint32_t index_{ 0 };
    };*/

}
