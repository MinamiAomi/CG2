#include "DescriptorHeap.h"

#include <bit>
#include <cassert>
#include <memory>

#include "Device.h"

namespace CG::DX12 {

    void DescriptorHeap::Initialize(const Device& device, uint32_t descriptorCount, D3D12_DESCRIPTOR_HEAP_TYPE type) {
        assert(device.IsEnabled());
        assert(descriptorCount > 0);
        assert(type != D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES);

        descriptorHeap_.Reset();

        D3D12_DESCRIPTOR_HEAP_DESC desc{};
        desc.Type = type;
        desc.NumDescriptors = descriptorCount;
        if (type == D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) {
            desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        }

        if (device.GetDevice()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(descriptorHeap_.GetAddressOf()))) {
            assert(false);
        }

        type_ = type;
        cpuHandleStart_ = descriptorHeap_->GetCPUDescriptorHandleForHeapStart();
        gpuHandleStart_ = {};
        if (ShaderVisible()) {
            gpuHandleStart_ = descriptorHeap_->GetGPUDescriptorHandleForHeapStart();
        }
        descriptorCount_ = descriptorCount;
        descriptorSize_ = device.GetDevice()->GetDescriptorHandleIncrementSize(type_);
        useTable_.Resize(descriptorCount_, false);
        lastAllocatedIndex_ = 0;
    }

    Descriptor DescriptorHeap::Allocate() {
        assert(IsEnabled());
        assert(!useTable_.All(true));
        uint32_t allocatedIndex = static_cast<uint32_t>(useTable_.FindNext(lastAllocatedIndex_, false));
        if (allocatedIndex >= descriptorCount_) {
            allocatedIndex = static_cast<uint32_t>(useTable_.FindFirst(false));
            assert(allocatedIndex < descriptorCount_);
        }
        useTable_.Set(allocatedIndex, true);
        lastAllocatedIndex_ = allocatedIndex;

        Descriptor descriptor{};
        descriptor.data_ = std::make_shared<Descriptor::Data>();
        descriptor.data_->descriptorHeap = this;
        descriptor.data_->index = allocatedIndex;
        descriptor.data_->cpuHandle = cpuHandleStart_;
        descriptor.data_->cpuHandle.ptr += static_cast<uint64_t>(descriptorSize_) * allocatedIndex;
        if (ShaderVisible()) {
            descriptor.data_->gpuHandle = gpuHandleStart_;
            descriptor.data_->gpuHandle.ptr += static_cast<uint64_t>(descriptorSize_) * allocatedIndex;
        }
        return descriptor;
    }

    void DescriptorHeap::Deallocate(Descriptor::Data* data) {
        assert(data->descriptorHeap == this);
        assert(useTable_[data->index]);

        useTable_.Set(data->index, false);
    }

    void DescriptorHeap::Deallocate(Descriptor* descriptor) {
        assert(descriptor->data_->descriptorHeap == this);
        assert(useTable_[descriptor->data_->index]);
        assert(descriptor->IsEnabled());

        useTable_.Set(descriptor->data_->index, false);
    }

    void DescriptorHeap::UseTable::Resize(size_t count, bool value) {
        const size_t blockCount = (count + kBlockSize - 1) / kBlockSize;
        count_ = count;
        table_.resize(blockCount, value ? ~0ull : 0ull);
    }
    size_t DescriptorHeap::UseTable::FindFirst(bool value) {
        for (size_t blockIndex = 0; blockIndex < table_.size(); ++blockIndex) {
            if (table_[blockIndex] == (value ? 0ull : ~0ull)) { continue; }
            size_t index = static_cast<size_t>(value ? std::countr_zero(table_[blockIndex]) : std::countr_one(table_[blockIndex]));
            index += blockIndex * kBlockSize;
            if (index < count_) {
                return index;
            }
            return count_;
        }
        return count_;
    }
    size_t DescriptorHeap::UseTable::FindNext(size_t index, bool value) {
        if (index > count_) { return count_; }

        const size_t startBlock = index >> kToBlockIndex;

        size_t indexInBlock = index & kBlockMask;
        for (size_t blockIndex = startBlock; blockIndex < table_.size(); ++blockIndex) {
            if (table_[blockIndex] == (value ? 0ull : ~0ull)) { continue; }
            uint64_t searchBlock = table_[blockIndex] >> indexInBlock;
            uint64_t searchScope = kBlockSize - indexInBlock;
            size_t searchIndex = static_cast<size_t>(value ? std::countr_zero(searchBlock) : std::countr_one(searchBlock));
            if (searchIndex < searchScope) {
                searchIndex += blockIndex * kBlockSize + indexInBlock;
                if (searchIndex < count_) { return searchIndex; }
                return count_;
            }
            indexInBlock = 0;
        }

        return count_;
    }
    void DescriptorHeap::UseTable::Set(size_t index, bool value) {
        if (value) {
            table_[index >> kToBlockIndex] |= (1ull << (index & kBlockMask));
        }
        else {
            table_[index >> kToBlockIndex] &= ~(1ull << (index & kBlockMask));
        }
    }

    bool DescriptorHeap::UseTable::operator[](size_t index) {
        return (table_[index >> kToBlockIndex] & (1ull << (index & kBlockMask))) != 0;
    }
    bool DescriptorHeap::UseTable::All(bool value) {
        // 不必要なビットをそろえる
        if (value) {
            table_.back() |= (~0ull << count_);
        }
        else {
            table_.back() &= ~(~0ull << count_);
        }

        for (uint64_t block : table_) {
            if (block != (value ? ~0ull : 0ull)) { return false; }
        }
        return true;
    }
}