#include "ResourceStateTracker.h"

#include "../Externals/DirectXTex/d3dx12.h"

#include "Defines.h"
#include "CommandList.h"
#include "Resource.h"

ResourceStateTracker::ResourceStateMap ResourceStateTracker::globalResourceState_;
std::mutex ResourceStateTracker::globalMutex_;
bool ResourceStateTracker::isLocked_ = false;

void ResourceStateTracker::Lock() {
    globalMutex_.lock();
    isLocked_ = true;
}

void ResourceStateTracker::Unlock() {
    globalMutex_.unlock();
    isLocked_ = false;
}

void ResourceStateTracker::AddGlobalResourceState(ID3D12Resource* resource, D3D12_RESOURCE_STATES state) {
    if (resource) {
        std::lock_guard<std::mutex> lock(globalMutex_);
        globalResourceState_[resource].SetSubresourceState(D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE, state);
    }
}
void ResourceStateTracker::RemoveGlobalResourceState(ID3D12Resource* resource) {
    if (resource) {
        std::lock_guard<std::mutex> lock(globalMutex_);
        globalResourceState_.erase(resource);
    }
}

void ResourceStateTracker::ResourceBarrier(const D3D12_RESOURCE_BARRIER& barrier) {
    if (barrier.Type == D3D12_RESOURCE_BARRIER_TYPE_TRANSITION) {
        auto& transitionBarrier = barrier.Transition;

        // 既にコマンドリストで使用されたことがあるリソースか探す
        const auto iter = finalResourceState_.find(transitionBarrier.pResource);
        if (iter != finalResourceState_.end()) {
            auto& resourceState = iter->second;

            // 最終的なリソースの状態が違う
            if (transitionBarrier.Subresource == D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES &&
                !resourceState.subresourceStateMap.empty()) {

                // サブリソースのStateAfterが違う場合遷移させる
                for (auto subresourceState : resourceState.subresourceStateMap) {
                    if (transitionBarrier.StateAfter != subresourceState.second) {
                        D3D12_RESOURCE_BARRIER newBarrier = barrier;
                        newBarrier.Transition.Subresource = subresourceState.first;
                        newBarrier.Transition.StateBefore = subresourceState.second;
                        resourceBarriers_.emplace_back(newBarrier);
                    }
                }
            }
            else {
                auto finalState = resourceState.GetSubresourceState(transitionBarrier.Subresource);
                if (transitionBarrier.StateAfter != finalState) {
                    // Beforeを正しくする
                    D3D12_RESOURCE_BARRIER newBarrier = barrier;
                    newBarrier.Transition.StateBefore = finalState;
                    resourceBarriers_.emplace_back(newBarrier);
                }
            }
        }
        else { // リソースは始めて使用される
            pendingResourceBarriers_.emplace_back(barrier);
        }

        // 最終的な状態を保存しておく
        finalResourceState_[transitionBarrier.pResource].SetSubresourceState(transitionBarrier.Subresource, transitionBarrier.StateAfter);
    }
    else {
        // 遷移バリアではないので追加するだけ
        resourceBarriers_.emplace_back(barrier);
    }
}

void ResourceStateTracker::TransitionResource(ID3D12Resource* resource, D3D12_RESOURCE_STATES stateAfter, uint32_t subresource) {
    if (resource) {
        ResourceBarrier(CD3DX12_RESOURCE_BARRIER::Transition(resource, D3D12_RESOURCE_STATE_COMMON, stateAfter, subresource));
    }
}

void ResourceStateTracker::TransitionResource(const GPUResource& resource, D3D12_RESOURCE_STATES stateAfter, uint32_t subresource) {
    TransitionResource(resource.GetResource().Get(), stateAfter, subresource);
}

void ResourceStateTracker::UAVBarrier(const GPUResource* resource) {
    ID3D12Resource* pResource = (resource) ? resource->GetResource().Get() : nullptr;
    ResourceBarrier(CD3DX12_RESOURCE_BARRIER::UAV(pResource));
}

void ResourceStateTracker::AliasBarrier(const GPUResource* resourceBefore, const GPUResource* resourceAfter) {
    ID3D12Resource* pResourceBefore = (resourceBefore) ? resourceBefore->GetResource().Get() : nullptr;
    ID3D12Resource* pResourceAfter = (resourceAfter) ? resourceAfter->GetResource().Get() : nullptr;
    ResourceBarrier(CD3DX12_RESOURCE_BARRIER::Aliasing(pResourceBefore, pResourceAfter));
}

uint32_t ResourceStateTracker::FlushPendingResourceBarriers(CommandList& commandList) {
    ASSERT(isLocked_);

    // グローバルの状態を確認し保留中のリソースバリアを修正する
    // 保留中とグローバルが一致しない場合バリアを追加
    ResourceBarriers resourceBarriers;
    resourceBarriers.reserve(pendingResourceBarriers_.size());

    for (D3D12_RESOURCE_BARRIER pendingBarrier : pendingResourceBarriers_) {

        if (pendingBarrier.Type == D3D12_RESOURCE_BARRIER_TYPE_TRANSITION) {

            auto pendingTransition = pendingBarrier.Transition;
            const auto iter = globalResourceState_.find(pendingTransition.pResource);
            if (iter != globalResourceState_.end()) {
               
                auto& resourceState = iter->second;
                if (pendingTransition.Subresource == D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES &&
                    !resourceState.subresourceStateMap.empty()) {
                    // すべてのサブリソースを遷移
                    for (const auto& subresourceState : resourceState.subresourceStateMap) {
                        if (pendingTransition.StateAfter != subresourceState.second) {
                            D3D12_RESOURCE_BARRIER newBarrier = pendingBarrier;
                            newBarrier.Transition.Subresource = subresourceState.first;
                            newBarrier.Transition.StateBefore = subresourceState.second;
                            resourceBarriers.emplace_back(newBarrier);
                        }
                    }
                }
                else {
                    auto globalState = iter->second.GetSubresourceState(pendingTransition.Subresource);
                    if (pendingTransition.StateAfter != globalState) {
                        // グローバルに基づいて修正
                        pendingTransition.StateBefore = globalState;
                        resourceBarriers.emplace_back(pendingBarrier);
                    }
                }
            }
        }
    }

    uint32_t numBarriers = static_cast<uint32_t>(resourceBarriers.size());
    if (numBarriers > 0) {
        auto graphicsCommandList = commandList.GetGraphicsCommandList();
        graphicsCommandList->ResourceBarrier(numBarriers, resourceBarriers.data());
    }
    pendingResourceBarriers_.clear();
    return numBarriers;
}

void ResourceStateTracker::FlushResourceBarriers(CommandList& commandList) {
    uint32_t numBarriers = static_cast<uint32_t>(resourceBarriers_.size());
    if (numBarriers > 0) {
        auto graphicsCommandList = commandList.GetGraphicsCommandList();
        graphicsCommandList->ResourceBarrier(numBarriers, resourceBarriers_.data());
        resourceBarriers_.clear();
    }
}

void ResourceStateTracker::CommitFinalResourceStates() {
    ASSERT(isLocked_);

    // 最終的なリソースの状態を保存
    for (const auto& resourceState : finalResourceState_) {
        globalResourceState_[resourceState.first] = resourceState.second;
    }
    finalResourceState_.clear();
}

void ResourceStateTracker::Reset() {
    pendingResourceBarriers_.clear();
    resourceBarriers_.clear();
    finalResourceState_.clear();
}
