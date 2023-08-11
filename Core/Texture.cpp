#include "Texture.h"

#include "../Externals/DirectXTex/d3dx12.h"

#include "Defines.h"
#include "Utility.h"
#include "Graphics.h"
#include "ResourceStateTracker.h"

namespace {
    D3D12_UNORDERED_ACCESS_VIEW_DESC GetUAVDesc(const D3D12_RESOURCE_DESC& resourceDesc, uint32_t mipSlice, uint32_t arraySlice = 0, uint32_t planeSlice = 0) {
        D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
        uavDesc.Format = resourceDesc.Format;

        switch (resourceDesc.Dimension) {
        case D3D12_RESOURCE_DIMENSION_TEXTURE1D:
            if (resourceDesc.DepthOrArraySize > 1) {
                uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE1DARRAY;
                uavDesc.Texture1DArray.ArraySize = resourceDesc.DepthOrArraySize - arraySlice;
                uavDesc.Texture1DArray.FirstArraySlice = arraySlice;
                uavDesc.Texture1DArray.MipSlice = mipSlice;
            }
            else {
                uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE1D;
                uavDesc.Texture1D.MipSlice = mipSlice;
            }
            break;
        case D3D12_RESOURCE_DIMENSION_TEXTURE2D:
            if (resourceDesc.DepthOrArraySize > 1) {
                uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
                uavDesc.Texture2DArray.ArraySize = resourceDesc.DepthOrArraySize - arraySlice;
                uavDesc.Texture2DArray.FirstArraySlice = arraySlice;
                uavDesc.Texture2DArray.PlaneSlice = planeSlice;
                uavDesc.Texture2DArray.MipSlice = mipSlice;
            }
            else {
                uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
                uavDesc.Texture2D.PlaneSlice = planeSlice;
                uavDesc.Texture2D.MipSlice = mipSlice;
            }
            break;
            break;
        case D3D12_RESOURCE_DIMENSION_TEXTURE3D:
            uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE3D;
            uavDesc.Texture3D.WSize = resourceDesc.DepthOrArraySize - arraySlice;
            uavDesc.Texture3D.FirstWSlice = arraySlice;
            uavDesc.Texture3D.MipSlice = mipSlice;
            break;
        default:
            ASSERT(false);
        }
        return uavDesc;
    }

}

bool Texture::IsUAVCompatibleFormat(DXGI_FORMAT format) {
    switch (format) {
    case DXGI_FORMAT_R32G32B32A32_FLOAT:
    case DXGI_FORMAT_R32G32B32A32_UINT:
    case DXGI_FORMAT_R32G32B32A32_SINT:
    case DXGI_FORMAT_R16G16B16A16_FLOAT:
    case DXGI_FORMAT_R16G16B16A16_UINT:
    case DXGI_FORMAT_R16G16B16A16_SINT:
    case DXGI_FORMAT_R8G8B8A8_UNORM:
    case DXGI_FORMAT_R8G8B8A8_UINT:
    case DXGI_FORMAT_R8G8B8A8_SINT:
    case DXGI_FORMAT_R32_FLOAT:
    case DXGI_FORMAT_R32_UINT:
    case DXGI_FORMAT_R32_SINT:
    case DXGI_FORMAT_R16_FLOAT:
    case DXGI_FORMAT_R16_UINT:
    case DXGI_FORMAT_R16_SINT:
    case DXGI_FORMAT_R8_UNORM:
    case DXGI_FORMAT_R8_UINT:
    case DXGI_FORMAT_R8_SINT:
        return true;
    default:
    }
    return false;
}

bool Texture::IsSRGBFormat(DXGI_FORMAT format) {
    switch (format) {
    case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
    case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
    case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
        return true;
    default:
    }
    return false;
}

bool Texture::IsBGRFormat(DXGI_FORMAT format) {
    switch (format) {
    case DXGI_FORMAT_B8G8R8A8_UNORM:
    case DXGI_FORMAT_B8G8R8X8_UNORM:
    case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
    case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
        return true;
    default:
    }
    return false;
}

bool Texture::IsDepthFormat(DXGI_FORMAT format) {
    switch (format) {
    case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
    case DXGI_FORMAT_D32_FLOAT:
    case DXGI_FORMAT_D24_UNORM_S8_UINT:
    case DXGI_FORMAT_D16_UNORM:
        return true;
    default:
    }
    return false;
}

DXGI_FORMAT Texture::GetTypelessFormat(DXGI_FORMAT format) {
    DXGI_FORMAT typelessFormat = format;

    switch (format) {
    case DXGI_FORMAT_R32G32B32A32_FLOAT:
    case DXGI_FORMAT_R32G32B32A32_UINT:
    case DXGI_FORMAT_R32G32B32A32_SINT:
        typelessFormat = DXGI_FORMAT_R32G32B32A32_TYPELESS;
        break;
    case DXGI_FORMAT_R32G32B32_FLOAT:
    case DXGI_FORMAT_R32G32B32_UINT:
    case DXGI_FORMAT_R32G32B32_SINT:
        typelessFormat = DXGI_FORMAT_R32G32B32_TYPELESS;
        break;
    case DXGI_FORMAT_R16G16B16A16_FLOAT:
    case DXGI_FORMAT_R16G16B16A16_UNORM:
    case DXGI_FORMAT_R16G16B16A16_UINT:
    case DXGI_FORMAT_R16G16B16A16_SNORM:
    case DXGI_FORMAT_R16G16B16A16_SINT:
        typelessFormat = DXGI_FORMAT_R16G16B16A16_TYPELESS;
        break;
    case DXGI_FORMAT_R32G32_FLOAT:
    case DXGI_FORMAT_R32G32_UINT:
    case DXGI_FORMAT_R32G32_SINT:
        typelessFormat = DXGI_FORMAT_R32G32_TYPELESS;
        break;
    case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
        typelessFormat = DXGI_FORMAT_R32G8X24_TYPELESS;
        break;
    case DXGI_FORMAT_R10G10B10A2_UNORM:
    case DXGI_FORMAT_R10G10B10A2_UINT:
        typelessFormat = DXGI_FORMAT_R10G10B10A2_TYPELESS;
        break;
    case DXGI_FORMAT_R8G8B8A8_UNORM:
    case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
    case DXGI_FORMAT_R8G8B8A8_UINT:
    case DXGI_FORMAT_R8G8B8A8_SNORM:
    case DXGI_FORMAT_R8G8B8A8_SINT:
        typelessFormat = DXGI_FORMAT_R8G8B8A8_TYPELESS;
        break;
    case DXGI_FORMAT_R16G16_FLOAT:
    case DXGI_FORMAT_R16G16_UNORM:
    case DXGI_FORMAT_R16G16_UINT:
    case DXGI_FORMAT_R16G16_SNORM:
    case DXGI_FORMAT_R16G16_SINT:
        typelessFormat = DXGI_FORMAT_R16G16_TYPELESS;
        break;
    case DXGI_FORMAT_D32_FLOAT:
    case DXGI_FORMAT_R32_FLOAT:
    case DXGI_FORMAT_R32_UINT:
    case DXGI_FORMAT_R32_SINT:
        typelessFormat = DXGI_FORMAT_R32_TYPELESS;
        break;
    case DXGI_FORMAT_R8G8_UNORM:
    case DXGI_FORMAT_R8G8_UINT:
    case DXGI_FORMAT_R8G8_SNORM:
    case DXGI_FORMAT_R8G8_SINT:
        typelessFormat = DXGI_FORMAT_R8G8_TYPELESS;
        break;
    case DXGI_FORMAT_R16_FLOAT:
    case DXGI_FORMAT_D16_UNORM:
    case DXGI_FORMAT_R16_UNORM:
    case DXGI_FORMAT_R16_UINT:
    case DXGI_FORMAT_R16_SNORM:
    case DXGI_FORMAT_R16_SINT:
        typelessFormat = DXGI_FORMAT_R16_TYPELESS;
        break;
    case DXGI_FORMAT_R8_UNORM:
    case DXGI_FORMAT_R8_UINT:
    case DXGI_FORMAT_R8_SNORM:
    case DXGI_FORMAT_R8_SINT:
    case DXGI_FORMAT_A8_UNORM:
        typelessFormat = DXGI_FORMAT_R8_TYPELESS;
        break;
    case DXGI_FORMAT_BC1_UNORM:
    case DXGI_FORMAT_BC1_UNORM_SRGB:
        typelessFormat = DXGI_FORMAT_BC1_TYPELESS;
        break;
    case DXGI_FORMAT_BC2_UNORM:
    case DXGI_FORMAT_BC2_UNORM_SRGB:
        typelessFormat = DXGI_FORMAT_BC2_TYPELESS;
        break;
    case DXGI_FORMAT_BC3_UNORM:
    case DXGI_FORMAT_BC3_UNORM_SRGB:
        typelessFormat = DXGI_FORMAT_BC3_TYPELESS;
        break;
    case DXGI_FORMAT_BC4_UNORM:
    case DXGI_FORMAT_BC4_SNORM:
        typelessFormat = DXGI_FORMAT_BC4_TYPELESS;
        break;
    case DXGI_FORMAT_BC5_UNORM:
    case DXGI_FORMAT_BC5_SNORM:
        typelessFormat = DXGI_FORMAT_BC5_TYPELESS;
        break;
    case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
        typelessFormat = DXGI_FORMAT_B8G8R8A8_TYPELESS;
        break;
    case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
        typelessFormat = DXGI_FORMAT_B8G8R8X8_TYPELESS;
        break;
    case DXGI_FORMAT_BC6H_UF16:
    case DXGI_FORMAT_BC6H_SF16:
        typelessFormat = DXGI_FORMAT_BC6H_TYPELESS;
        break;
    case DXGI_FORMAT_BC7_UNORM:
    case DXGI_FORMAT_BC7_UNORM_SRGB:
        typelessFormat = DXGI_FORMAT_BC7_TYPELESS;
        break;
    }

    return typelessFormat;
}

Texture::Texture(TextureUsage textureUsage, const std::wstring& name) :
    GPUResource(name),
    textureUsage_(textureUsage) {
}

Texture::Texture(const D3D12_RESOURCE_DESC& resourceDesc, const D3D12_CLEAR_VALUE* clearValue, TextureUsage textureUsage, const std::wstring& name) :
    GPUResource(resourceDesc, clearValue, name),
    textureUsage_(textureUsage) {
    CreateViews();
}

Texture::Texture(Microsoft::WRL::ComPtr<ID3D12Resource> resource, TextureUsage textureUsage, const std::wstring& name) :
    GPUResource(resource, name),
    textureUsage_(textureUsage) {
    CreateViews();
}

Texture::Texture(const Texture& copy) :
    GPUResource(copy) {
    CreateViews();
}

Texture& Texture::operator=(const Texture& copy) {
    GPUResource::operator=(copy);
    CreateViews();
    return *this;
}

Texture::Texture(Texture&& move) :
    GPUResource(std::forward<Texture>(move)) {
    CreateViews();
}

Texture& Texture::operator=(Texture&& move) {
    GPUResource::operator=(std::forward<Texture>(move));
    CreateViews();
    return *this;
}

Texture::~Texture() {
}

void Texture::CreateViews() {
    if (resource_) {
        auto& graphics = Graphics::Get();
        auto device = graphics.GetDevice();

        CD3DX12_RESOURCE_DESC desc(resource_->GetDesc());

        D3D12_FEATURE_DATA_FORMAT_SUPPORT formatSupport{};
        formatSupport.Format = desc.Format;
        ASSERT_IF_FAILED(device->CheckFeatureSupport(D3D12_FEATURE_FORMAT_SUPPORT, &formatSupport, sizeof(formatSupport)));

        if ((desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET) != 0 &&
            CheckRTVSupport(formatSupport.Support1)) {
            rtv_ = graphics.AllocateDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
            device->CreateRenderTargetView(resource_.Get(), nullptr, rtv_.GetDescriptorHandle());
        }
        if ((desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL) != 0 &&
            CheckDSVSupport(formatSupport.Support1)) {
            dsv_ = graphics.AllocateDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
            device->CreateDepthStencilView(resource_.Get(), nullptr, rtv_.GetDescriptorHandle());
        }
    }

    std::lock_guard<std::mutex> srvLock(srvMutex_);
    std::lock_guard<std::mutex> uavLock(uavMutex_);

    srvs_.clear();
    uavs_.clear();
}

D3D12_CPU_DESCRIPTOR_HANDLE Texture::GetShaderResourceView(const D3D12_SHADER_RESOURCE_VIEW_DESC* srvDesc) const {
    size_t hash = 0;
    if (srvDesc) {
        hash = std::hash<D3D12_SHADER_RESOURCE_VIEW_DESC>{}(*srvDesc);
    }

    std::lock_guard<std::mutex> lock(srvMutex_);

    auto iter = srvs_.find(hash);
    if (iter == srvs_.end()) {
        auto srv = CreateSRV(srvDesc);
        iter = srvs_.insert({ hash, std::move(srv) }).first;
    }

    return iter->second.GetDescriptorHandle();
}

D3D12_CPU_DESCRIPTOR_HANDLE Texture::GetUnorderedAccessView(const D3D12_UNORDERED_ACCESS_VIEW_DESC* uavDesc) const {
    size_t hash = 0;
    if (uavDesc) {
        hash = std::hash<D3D12_UNORDERED_ACCESS_VIEW_DESC>{}(*uavDesc);
    }

    std::lock_guard<std::mutex> lock(uavMutex_);

    auto iter = uavs_.find(hash);
    if (iter == uavs_.end()) {
        auto uav = CreateUAV(uavDesc);
        iter = uavs_.insert({ hash, std::move(uav) }).first;
    }

    return iter->second.GetDescriptorHandle();
}

void Texture::Resize(uint32_t width, uint32_t height, uint32_t depthOrArraySize) {
    if (resource_) {
        ResourceStateTracker::RemoveGlobalResourceState(resource_.Get());

        CD3DX12_RESOURCE_DESC resourceDesc(resource_->GetDesc());

        resourceDesc.Width = std::max(width, 1u);
        resourceDesc.Height = std::max(height, 1u);
        resourceDesc.DepthOrArraySize = depthOrArraySize;

        auto device = Graphics::Get().GetDevice();

        ASSERT_IF_FAILED(device->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
            D3D12_HEAP_FLAG_NONE,
            &resourceDesc,
            D3D12_RESOURCE_STATE_COMMON,
            clearValue_.get(),
            IID_PPV_ARGS(resource_.GetAddressOf())));

#ifdef _DEBUG
        resource_->SetName(name_.c_str());
#endif // _DEBUG

        ResourceStateTracker::AddGlobalResourceState(resource_.Get(), D3D12_RESOURCE_STATE_COMMON);

        CreateViews();
    }
}

DescriptorAllocation Texture::CreateSRV(const D3D12_SHADER_RESOURCE_VIEW_DESC* srvDesc) const {
    auto& graphics = Graphics::Get();
    auto device = graphics.GetDevice();
    auto srv = graphics.AllocateDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    device->CreateShaderResourceView(resource_.Get(), srvDesc, srv.GetDescriptorHandle());

    return srv;
}

DescriptorAllocation Texture::CreateUAV(const D3D12_UNORDERED_ACCESS_VIEW_DESC* uavDesc) const {
    auto& graphics = Graphics::Get();
    auto device = graphics.GetDevice();
    auto uav = graphics.AllocateDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    device->CreateUnorderedAccessView(resource_.Get(), nullptr, uavDesc, uav.GetDescriptorHandle());

    return uav;
}