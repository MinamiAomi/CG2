#pragma once

#include <d3d12.h>

#include <cstdint>
#include <map>
#include <mutex>
#include <unordered_map>
#include <vector>

class CommandList;
class GPUResource;

class ResourceStateTracker {
public:
    static void Lock();
    static void Unlock();
    static void AddGlobalResourceState(ID3D12Resource* resource, D3D12_RESOURCE_STATES state);
    static void RemoveGlobalResourceState(ID3D12Resource* resource);


    void ResourceBarrier(const D3D12_RESOURCE_BARRIER& barrier);

    void TransitionResource(ID3D12Resource* resource, D3D12_RESOURCE_STATES stateAfter, uint32_t subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES);
    void TransitionResource(const GPUResource& resource, D3D12_RESOURCE_STATES stateAfter, uint32_t subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES);
    void UAVBarrier(const GPUResource* resource = nullptr);
    void AliasBarrier(const GPUResource* resourceBefore = nullptr, const GPUResource* resourceAfter = nullptr);

    uint32_t FlushPendingResourceBarriers(CommandList& commandList);
    void FlushResourceBarriers(CommandList& commandList);
    void CommitFinalResourceStates();

    void Reset();

private:
    // リソースバリア配列
    using ResourceBarriers = std::vector<D3D12_RESOURCE_BARRIER>;
    
    // リソースの状態とそのサブリソースの状態を管理
    struct ResourceState {
        explicit ResourceState(D3D12_RESOURCE_STATES state = D3D12_RESOURCE_STATE_COMMON) : state(state) {}

        void SetSubresourceState(uint32_t subresource, D3D12_RESOURCE_STATES subresourceState) {
            if (subresource == D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES) {
                state = subresourceState;
                subresourceStateMap.clear();
            }
            else {
                subresourceStateMap[subresource] = subresourceState;
            }
        }

        D3D12_RESOURCE_STATES GetSubresourceState(uint32_t subresource) const {
            D3D12_RESOURCE_STATES subresourceState = state;
            const auto iter = subresourceStateMap.find(subresource);
            if (iter != subresourceStateMap.end()) {
                subresourceState = iter->second;
            }
            return subresourceState;
        }

        D3D12_RESOURCE_STATES state;
        // 空の場合すべてのサブリソースがリソースの状態
        std::map<uint32_t, D3D12_RESOURCE_STATES> subresourceStateMap;
    };

    using ResourceStateMap = std::unordered_map<ID3D12Resource*, ResourceState>;

    // コマンドリストで使用中のリソースの状態を保存
    static ResourceStateMap globalResourceState_;
    static std::mutex globalMutex_;
    static bool isLocked_;
    
    ResourceBarriers pendingResourceBarriers_;
    ResourceBarriers resourceBarriers_;
    // コマンドリスト内で使用されるリソースの状態
    // コマンドリストの実行時globalResourceState_に保存される
    ResourceStateMap finalResourceState_;
};