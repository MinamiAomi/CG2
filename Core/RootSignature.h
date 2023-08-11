#pragma once

#include <d3d12.h>
#include <wrl/client.h>

#include <cstdint>

class RootSignature {
public:
    RootSignature();
    RootSignature(const D3D12_ROOT_SIGNATURE_DESC1& rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION rootSignatureVersion);
    ~RootSignature();

    void Create(const D3D12_ROOT_SIGNATURE_DESC1& rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION rootSignatureVersion);
    void Destroy();

    Microsoft::WRL::ComPtr<ID3D12RootSignature> GetRootSignature() const { return rootSignature_; }
    const D3D12_ROOT_SIGNATURE_DESC1& GetRootSignatureDesc() const { return rootSignatureDesc_; }
    uint32_t GetDescriptorTableBitMask(D3D12_DESCRIPTOR_HEAP_TYPE descriptorHeapType) const;
    uint32_t GetNumDescriptors(uint32_t rootIndex) const;

private:
    // 32ビットのマスクを使用しているため
    // 最大32個のディスクリプタテーブルを使用可能
    static const uint32_t kMaxDescriptorTables = 32;

    D3D12_ROOT_SIGNATURE_DESC1 rootSignatureDesc_;
    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;

    // ディスクリプタテーブル当たりのディスクリプタの数
    uint32_t numDescriptorsPerTable_[kMaxDescriptorTables];
    // サンプラーのディスクリプタテーブルを表すマスク
    uint32_t samplerTableBitMask_;
    // CBVSRVUAVのディスクリプタテーブルを表すマスク
    uint32_t descriptorTableBitMask_;
};