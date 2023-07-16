#pragma once

#include <d3d12.h>
#include <wrl/client.h>

#include <cstdint>
#include <vector>

#include "Descriptor.h"

namespace CG::DX12 {
    using namespace Microsoft::WRL;

    class Device;

    class DescriptorHeap {
    public:
        void Initialize(const Device& device, uint32_t descriptorCount, D3D12_DESCRIPTOR_HEAP_TYPE type);

        Descriptor Allocate();
        void Deallocate(Descriptor::Data* data);
        void Deallocate(Descriptor* handle);

        bool IsEnabled() const { return descriptorHeap_; }
        bool ShaderVisible() const { return type_ == D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV; }
        ComPtr<ID3D12DescriptorHeap> GetDescriptorHeap() const { return descriptorHeap_; }
        D3D12_DESCRIPTOR_HEAP_TYPE GetType() const { return type_; }

    private:
        class UseTable {
        public:
            void Resize(size_t count, bool value);
            size_t FindFirst(bool value);
            size_t FindNext(size_t index, bool value);
            void Set(size_t index, bool value);
            bool operator[](size_t index);
            bool All(bool value);
            size_t Count() const { return count_; }

        private:
            static const size_t kBlockSize = sizeof(uint64_t) * CHAR_BIT;
            static const size_t kBlockMask = kBlockSize - 1;
            static const size_t kToBlockIndex = 6;

            std::vector<uint64_t> table_;
            size_t count_{ 0 };
        };

        ComPtr<ID3D12DescriptorHeap> descriptorHeap_;
        D3D12_CPU_DESCRIPTOR_HANDLE cpuHandleStart_{};
        D3D12_GPU_DESCRIPTOR_HANDLE gpuHandleStart_{};
        uint32_t descriptorSize_{ 0 };
        uint32_t descriptorCount_{ 0 };
        D3D12_DESCRIPTOR_HEAP_TYPE type_{ D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES };
        UseTable useTable_;
        uint32_t lastAllocatedIndex_{ 0 };
    };

}
