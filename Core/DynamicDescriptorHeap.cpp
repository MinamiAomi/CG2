#include "DynamicDescriptorHeap.h"

#include "Defines.h"
#include "Graphics.h"
#include "CommandList.h"
#include "RootSignature.h"

DynamicDescriptorHeap::DynamicDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heapType, uint32_t numDescriptorsPerHeap) :
    heapType_(heapType_),
    numDescriptorsPerHeap_(numDescriptorsPerHeap),
    descriptorTableBitMask_(0),
    staleDescriptorTableBitMask_(0),
    currentCPUDescriptorHandle_(D3D12_CPU_DESCRIPTOR_HANDLE_NULL),
    currentGPUDescriptorHandle_(D3D12_GPU_DESCRIPTOR_HANDLE_NULL),
    numFreeHandles_(0) {
    descriptorSize_ = Graphics::Get().GetDevice()->GetDescriptorHandleIncrementSize(heapType_);
    descriptorHandleCache_ = std::make_unique<D3D12_CPU_DESCRIPTOR_HANDLE[]>(numDescriptorsPerHeap_);
}

void DynamicDescriptorHeap::StageDescriptors(uint32_t rootParameterIndex, uint32_t offset, uint32_t numDescriptors, const D3D12_CPU_DESCRIPTOR_HANDLE srcDescriptors) {
    ASSERT(numDescriptors <= numDescriptorsPerHeap_);
    ASSERT(rootParameterIndex < kMaxDescriptorTables);

    auto& descriptorTableCache = descriptorTableCache_[rootParameterIndex];

    ASSERT((offset + numDescriptors) <= descriptorTableCache.numDescriptors);

    D3D12_CPU_DESCRIPTOR_HANDLE* dstDescriptor = (descriptorTableCache.baseDescriptor + offset);
    for (uint32_t i = 0; i < numDescriptors; ++i) {
        dstDescriptor[i] = srcDescriptors;
        dstDescriptor[i].ptr += size_t(descriptorSize_ * i);
    }

    staleDescriptorTableBitMask_ |= (1 << rootParameterIndex);
}

void DynamicDescriptorHeap::CommitStagedDescriptors(CommandList& commandList, std::function<void(ID3D12GraphicsCommandList*, uint32_t, D3D12_GPU_DESCRIPTOR_HANDLE)> setFunc) {
    // コピーするディスクリプタの数
    uint32_t numDescriptorsToCommit = ComputeStaleDescriptorCount();

    if (numDescriptorsToCommit > 0) {
        auto device = Graphics::Get().GetDevice();
        auto graphicsCommandList = commandList.GetGraphicsCommandList().Get();
        ASSERT(graphicsCommandList);

        if (!currentDescriptorHeap_ || numFreeHandles_ < numDescriptorsToCommit) {
            currentDescriptorHeap_ == RequestDescriptorHeap();
            currentCPUDescriptorHandle_ = currentDescriptorHeap_->GetCPUDescriptorHandleForHeapStart();
            currentGPUDescriptorHandle_ = currentDescriptorHeap_->GetGPUDescriptorHandleForHeapStart();

            commandList.SetDescriptorHeap(heapType_, currentDescriptorHeap_.Get());

            staleDescriptorTableBitMask_ = descriptorTableBitMask_;
        }

        DWORD rootIndex{};
        while (_BitScanForward(&rootIndex, staleDescriptorTableBitMask_)) {
            uint32_t numSrcDescriptors = descriptorTableCache_[rootIndex].numDescriptors;
            auto pSrcDescriptorHandles = descriptorTableCache_[rootIndex].baseDescriptor;

            D3D12_CPU_DESCRIPTOR_HANDLE pDestDescriptorRangeStarts[] = {
                currentCPUDescriptorHandle_
            };
            uint32_t pDestDescriptorRangeSize[] = {
                numSrcDescriptors
            };

            device->CopyDescriptors(1, pDestDescriptorRangeStarts, pDestDescriptorRangeSize,
                numSrcDescriptors, pSrcDescriptorHandles, nullptr, heapType_);

            setFunc(graphicsCommandList, rootIndex, currentGPUDescriptorHandle_);

            currentCPUDescriptorHandle_.ptr += size_t(numSrcDescriptors * descriptorSize_);
            currentGPUDescriptorHandle_.ptr += size_t(numSrcDescriptors * descriptorSize_);
            // もう一度スキャンされないようビットを反転
            staleDescriptorTableBitMask_ ^= (1 << rootIndex);
        }
    }
}

void DynamicDescriptorHeap::CommitStagedDescriptorsForDraw(CommandList& commandList) {
    CommitStagedDescriptors(commandList, &ID3D12GraphicsCommandList::SetGraphicsRootDescriptorTable);
}

void DynamicDescriptorHeap::CommitStagedDescriptorsForDispatch(CommandList& commandList) {
    CommitStagedDescriptors(commandList, &ID3D12GraphicsCommandList::SetComputeRootDescriptorTable);
}

D3D12_GPU_DESCRIPTOR_HANDLE DynamicDescriptorHeap::CopyDescriptor(CommandList& commandList, D3D12_CPU_DESCRIPTOR_HANDLE cpuDescriptor) {
    if (!currentDescriptorHeap_ || numFreeHandles_ < 1) {
        currentDescriptorHeap_ = RequestDescriptorHeap();
        currentCPUDescriptorHandle_ = currentDescriptorHeap_->GetCPUDescriptorHandleForHeapStart();
        currentGPUDescriptorHandle_ = currentDescriptorHeap_->GetGPUDescriptorHandleForHeapStart();
        numFreeHandles_ = numDescriptorsPerHeap_;

        commandList.SetDescriptorHeap(heapType_, currentDescriptorHeap_.Get());

        staleDescriptorTableBitMask_ = descriptorTableBitMask_;
    }

    auto device = Graphics::Get().GetDevice();

    D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = currentGPUDescriptorHandle_;
    device->CopyDescriptorsSimple(1, currentCPUDescriptorHandle_, cpuDescriptor, heapType_);

    currentCPUDescriptorHandle_.ptr += size_t(descriptorSize_);
    currentGPUDescriptorHandle_.ptr += size_t(descriptorSize_);
    numFreeHandles_ -= 1;

    return gpuHandle;
}

void DynamicDescriptorHeap::ParseRootSignature(const RootSignature& rootSignature) {
    staleDescriptorTableBitMask_ = 0;

    auto& rootSignatureDesc = rootSignature.GetRootSignatureDesc();

    // ディスクリプタテーブルの構成を表すマスクを取得
    descriptorTableBitMask_ = rootSignature.GetDescriptorTableBitMask(heapType_);
    uint32_t descriptorTableBitMask = descriptorTableBitMask_;

    uint32_t currentOffset = 0;
    DWORD rootIndex{};
    while (_BitScanForward(&rootIndex, descriptorTableBitMask) && rootIndex < rootSignatureDesc.NumParameters) {

        uint32_t numDescriptors = rootSignature.GetNumDescriptors(rootIndex);

        auto& descriptorTableCache = descriptorTableCache_[rootIndex];
        descriptorTableCache.numDescriptors = numDescriptors;
        descriptorTableCache.baseDescriptor = descriptorHandleCache_.get() + currentOffset;

        currentOffset += numDescriptors;
        // もう一度スキャンされないようビットを反転
        descriptorTableBitMask ^= (1 << rootIndex);
    }

    ASSERT(currentOffset <= numDescriptorsPerHeap_);
}

void DynamicDescriptorHeap::Reset() {
    availableDescriptorHeaps_ = descriptorHeapPool_;
    currentDescriptorHeap_.Reset();
    currentCPUDescriptorHandle_ = D3D12_CPU_DESCRIPTOR_HANDLE_NULL;
    currentGPUDescriptorHandle_ = D3D12_GPU_DESCRIPTOR_HANDLE_NULL;
    numFreeHandles_ = 0;
    descriptorTableBitMask_ = 0;
    staleDescriptorTableBitMask_ = 0;
    for (uint32_t i = 0; i < kMaxDescriptorTables; ++i) {
        descriptorTableCache_[i].Reset();
    }
}

Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> DynamicDescriptorHeap::RequestDescriptorHeap() {
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap;
    if (!availableDescriptorHeaps_.empty()) {
        descriptorHeap = availableDescriptorHeaps_.front();
        availableDescriptorHeaps_.pop();
    }
    else {
        descriptorHeap = CreateDescriptorHeap();
        descriptorHeapPool_.push(descriptorHeap);
    }
    return descriptorHeap;
}

Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> DynamicDescriptorHeap::CreateDescriptorHeap() {
    auto device = Graphics::Get().GetDevice();

    D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
    descriptorHeapDesc.Type = heapType_;
    descriptorHeapDesc.NumDescriptors = numDescriptorsPerHeap_;
    descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap;
    ASSERT_IF_FAILED(device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(descriptorHeap.GetAddressOf())));

    NAME_D3D12_OBJECT(descriptorHeap);

    return descriptorHeap;
}

uint32_t DynamicDescriptorHeap::ComputeStaleDescriptorCount() const {
    uint32_t numStaleDescriptors = 0;
    DWORD i{};
    DWORD staleDescriptorsBitMask = staleDescriptorTableBitMask_;

    while (_BitScanForward(&i, staleDescriptorsBitMask)) {
        numStaleDescriptors += descriptorTableCache_[i].numDescriptors;
        staleDescriptorsBitMask ^= (1 << i);
    }

    return numStaleDescriptors;
}