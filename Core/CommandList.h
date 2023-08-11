#pragma once

#include <d3d12.h>
#include <wrl/client.h>

#include <map>
#include <memory>
#include <mutex>
#include <vector>

class GPUBuffer;
class ByteAddressBuffer;
class ConstantBuffer;
class DynamicDescriptorHeap;
class RenderTarget;
class GPUResource;
class ResourceStateTracker;
class StructuredBuffer;
class RootSignature;
class Texture;
class UploadBuffer;
class VertexBuffer;

class CommandList {
public:
    CommandList(D3D12_COMMAND_LIST_TYPE type);

    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> GetGraphicsCommandList() const;

    void SetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heapType, ID3D12DescriptorHeap* heap);


private:
    using TrackedObjects = std::vector<Microsoft::WRL::ComPtr<ID3D12Object>>;

    static std::map<std::wstring, ID3D12Resource*> textureCache_;
    static std::mutex textureCacheMutex_;

    const D3D12_COMMAND_LIST_TYPE type_;
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_;
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator_;
    // 
    std::shared_ptr<CommandList> computeCommandList_;
    // 動的なメッシュ、変更の多い定数バッファなどの便利
    std::unique_ptr<UploadBuffer> uploadBuffer_;
    // リソースの遷移を管理
    std::unique_ptr<ResourceStateTracker> resourceStateTracker_;
    // 描画に使用するディスクリプタをコミットする必要がある
    std::unique_ptr<DynamicDescriptorHeap> dynamicDescriptorHeaps_[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];
    // 現在バインドされているルートシグネチャ
    ID3D12RootSignature* rootSignature_;
    // 現在バインドされているディスクリプタヒープ
    ID3D12DescriptorHeap* descriptorHeaps_[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];

    // コマンドリストが実行中にオブジェクトが削除されないよう保存
    // リセットされると解放される
    TrackedObjects trackedObjects_;
};