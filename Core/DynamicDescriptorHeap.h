#pragma once

#include <d3d12.h>
#include <wrl/client.h>

#include <cstdint>
#include <functional>
#include <memory>
#include <queue>

class CommandList;
class RootSignature;

class DynamicDescriptorHeap {
public:
    DynamicDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heapType, uint32_t numDescriptorsPerHeap = 1024);

    // CommitStagedDescriptors...にてコピーされる
    void StageDescriptors(uint32_t rootParameterIndex, uint32_t offset, uint32_t numDescriptors, const D3D12_CPU_DESCRIPTOR_HANDLE srcDescriptors);

    // StageDescriptorsでステージングされたディスクリプタをコピーし
    // CommandListにディスクリプタヒープを設定する
    void CommitStagedDescriptors(CommandList& commandList, std::function<void(ID3D12GraphicsCommandList*, uint32_t, D3D12_GPU_DESCRIPTOR_HANDLE)> setFunc);
    void CommitStagedDescriptorsForDraw(CommandList& commandList);
    void CommitStagedDescriptorsForDispatch(CommandList& commandList);

    // 一つだけコピーする
    D3D12_GPU_DESCRIPTOR_HANDLE CopyDescriptor(CommandList& commandList, D3D12_CPU_DESCRIPTOR_HANDLE cpuDescriptor);

    // ディスクリプタテーブル内のディスクリプタの数を取得する
    void ParseRootSignature(const RootSignature& rootSignature);

    void Reset();

private:
    // 32ビットのマスクを使用しているため
    // 最大32個のディスクリプタテーブルを使用
    static const uint32_t kMaxDescriptorTables = 32;

    struct DescriptorTableCache {
        DescriptorTableCache() : numDescriptors(0), baseDescriptor(nullptr) {}
        void Reset() { numDescriptors = 0; baseDescriptor = nullptr; }

        uint32_t numDescriptors;
        D3D12_CPU_DESCRIPTOR_HANDLE* baseDescriptor;
    };

    using DescriptorHeapPool = std::queue<Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>>;

    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> RequestDescriptorHeap();
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap();

    uint32_t ComputeStaleDescriptorCount() const;

    // CBVSRVUAV,Samplerのみ可
    const D3D12_DESCRIPTOR_HEAP_TYPE heapType_;
    uint32_t numDescriptorsPerHeap_;
    uint32_t descriptorSize_;
    std::unique_ptr<D3D12_CPU_DESCRIPTOR_HANDLE[]> descriptorHandleCache_;
    DescriptorTableCache descriptorTableCache_[kMaxDescriptorTables];

    uint32_t descriptorTableBitMask_;
    uint32_t staleDescriptorTableBitMask_;

    DescriptorHeapPool descriptorHeapPool_;
    DescriptorHeapPool availableDescriptorHeaps_;

    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> currentDescriptorHeap_;
    D3D12_CPU_DESCRIPTOR_HANDLE currentCPUDescriptorHandle_;
    D3D12_GPU_DESCRIPTOR_HANDLE currentGPUDescriptorHandle_;

    uint32_t numFreeHandles_;
};