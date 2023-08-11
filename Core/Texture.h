#pragma once
#include "GPUResource.h"

#include <mutex>
#include <unordered_map>

#include "DescriptorAllocation.h"
#include "TextureUsage.h"

class Texture : public GPUResource {
public:
    static bool CheckSRVSupport(D3D12_FORMAT_SUPPORT1 formatSupport) { return ((formatSupport & D3D12_FORMAT_SUPPORT1_SHADER_SAMPLE) != 0 || (formatSupport & D3D12_FORMAT_SUPPORT1_SHADER_LOAD) != 0); }
    static bool CheckUAVSupport(D3D12_FORMAT_SUPPORT1 formatSupport) { return ((formatSupport & D3D12_FORMAT_SUPPORT1_TYPED_UNORDERED_ACCESS_VIEW) != 0); }
    static bool CheckRTVSupport(D3D12_FORMAT_SUPPORT1 formatSupport) { return ((formatSupport & D3D12_FORMAT_SUPPORT1_RENDER_TARGET) != 0); }
    static bool CheckDSVSupport(D3D12_FORMAT_SUPPORT1 formatSupport) { return ((formatSupport & D3D12_FORMAT_SUPPORT1_DEPTH_STENCIL) != 0); }

    static bool IsUAVCompatibleFormat(DXGI_FORMAT format);
    static bool IsSRGBFormat(DXGI_FORMAT format);
    static bool IsBGRFormat(DXGI_FORMAT format);
    static bool IsDepthFormat(DXGI_FORMAT format);

    static DXGI_FORMAT GetTypelessFormat(DXGI_FORMAT format);

    explicit Texture(TextureUsage textureUsage = TextureUsage::Albedo, const std::wstring& name = L"");
    explicit Texture(const D3D12_RESOURCE_DESC& resourceDesc, const D3D12_CLEAR_VALUE* clearValue = nullptr, TextureUsage textureUsage = TextureUsage::Albedo, const std::wstring& name = L"");
    explicit Texture(Microsoft::WRL::ComPtr<ID3D12Resource> resource, TextureUsage textureUsage = TextureUsage::Albedo, const std::wstring& name = L"");

    Texture(const Texture& copy);
    Texture& operator=(const Texture& copy);

    Texture(Texture&& move);
    Texture& operator=(Texture&& move);

    virtual ~Texture();

    virtual void CreateViews();

    virtual D3D12_CPU_DESCRIPTOR_HANDLE GetShaderResourceView(const D3D12_SHADER_RESOURCE_VIEW_DESC* srvDesc = nullptr) const override;
    virtual D3D12_CPU_DESCRIPTOR_HANDLE GetUnorderedAccessView(const D3D12_UNORDERED_ACCESS_VIEW_DESC* srvDesc = nullptr) const override;

    void Resize(uint32_t width, uint32_t height, uint32_t depthOrArraySize = 1);

    virtual D3D12_CPU_DESCRIPTOR_HANDLE GetRenderTargetView() const { return rtv_.GetDescriptorHandle(); }
    virtual D3D12_CPU_DESCRIPTOR_HANDLE GetDepthStencilView() const { return dsv_.GetDescriptorHandle(); }

    TextureUsage GetTextureUsage() const { return textureUsage_; }

    void SetTextureUsage(TextureUsage textureUsage) { textureUsage_ = textureUsage; }

private:
    DescriptorAllocation CreateSRV(const D3D12_SHADER_RESOURCE_VIEW_DESC* srvDesc) const;
    DescriptorAllocation CreateUAV(const D3D12_UNORDERED_ACCESS_VIEW_DESC* srvDesc) const;


    mutable std::unordered_map<size_t, DescriptorAllocation> srvs_;
    mutable std::unordered_map<size_t, DescriptorAllocation> uavs_;

    mutable std::mutex srvMutex_;
    mutable std::mutex uavMutex_;

    DescriptorAllocation rtv_;
    DescriptorAllocation dsv_;

    TextureUsage textureUsage_;
};