#pragma once

#include <d3d12.h>
#include <wrl/client.h>

#include <memory>
#include <string>

class GPUResource {
public:
    GPUResource(const std::wstring& name = L"");
    GPUResource(const D3D12_RESOURCE_DESC& resourceDesc, const D3D12_CLEAR_VALUE* clearValue = nullptr, const std::wstring& name = L"");
    GPUResource(Microsoft::WRL::ComPtr<ID3D12Resource> resource, const std::wstring& name = L"");

    GPUResource(const GPUResource& copy);
    GPUResource& operator=(const GPUResource& copy);

    GPUResource(GPUResource&& move);
    GPUResource& operator=(GPUResource&& move);

    virtual ~GPUResource();

    virtual void Reset();

    Microsoft::WRL::ComPtr<ID3D12Resource> GetResource() const { return resource_; }
    D3D12_RESOURCE_DESC GetResourceDesc() const;

    virtual D3D12_CPU_DESCRIPTOR_HANDLE GetShaderResourceView(const D3D12_SHADER_RESOURCE_VIEW_DESC* srvDesc = nullptr) const = 0;
    virtual D3D12_CPU_DESCRIPTOR_HANDLE GetUnorderedAccessView(const D3D12_UNORDERED_ACCESS_VIEW_DESC* uavDesc = nullptr) const = 0;

    virtual void SetResource(Microsoft::WRL::ComPtr<ID3D12Resource> resource, const D3D12_CLEAR_VALUE* clearValue = nullptr);
    void SetName(const std::wstring& name);

    bool IsValid() const { return resource_; }

protected:
    void Create(const D3D12_RESOURCE_DESC& resourceDesc, const D3D12_CLEAR_VALUE* clearValue = nullptr, const std::wstring& name = L"");
    
    Microsoft::WRL::ComPtr<ID3D12Resource> resource_;
    std::unique_ptr<D3D12_CLEAR_VALUE> clearValue_;

#ifdef _DEBUG
    // デバッグの役に立つ名前
    std::wstring name_;
#endif _DEBUG 
};