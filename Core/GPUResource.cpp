#include "GPUResource.h"

#include "../Externals/DirectXTex/d3dx12.h"

#include "Graphics.h"
#include "Defines.h"
#include "ResourceStateTracker.h"

GPUResource::GPUResource(const std::wstring& name) {
    SetName(name);
}

GPUResource::GPUResource(const D3D12_RESOURCE_DESC& resourceDesc, const D3D12_CLEAR_VALUE* clearValue, const std::wstring& name) {
    Create(resourceDesc, clearValue, name);
}

GPUResource::GPUResource(Microsoft::WRL::ComPtr<ID3D12Resource> resource, const std::wstring& name) :
    resource_(resource) {
    SetName(name);
}

GPUResource::GPUResource(const GPUResource& copy) :
    resource_(copy.resource_),
    clearValue_(copy.clearValue_ ? std::make_unique<D3D12_CLEAR_VALUE>(*copy.clearValue_) : nullptr) {
#ifdef  _DEBUG
    SetName(copy.name_);
#endif //  _DEBUG
}

GPUResource& GPUResource::operator=(const GPUResource& copy) {
    if (this != &copy) {
        resource_ = copy.resource_;
        if (copy.clearValue_) {
            clearValue_ = std::make_unique<D3D12_CLEAR_VALUE>(*copy.clearValue_);
        }
#ifdef  _DEBUG
        SetName(copy.name_);
#endif //  _DEBUG
    }
    return *this;
}

GPUResource::GPUResource(GPUResource&& move) :
    resource_(std::move(move.resource_)),
    clearValue_(std::move(move.clearValue_)) {
#ifdef  _DEBUG
    name_ = std::move(move.name_);
#endif //  _DEBUG
}


GPUResource& GPUResource::operator=(GPUResource&& move) {
    if (this != &move) {
        resource_ = move.resource_;
        clearValue_ = std::move(move.clearValue_);

        move.resource_.Reset();
#ifdef  _DEBUG
        name_ = std::move(move.name_);
        move.name_.clear();
#endif //  _DEBUG
    }
    return *this;
}

GPUResource::~GPUResource() {

}

D3D12_RESOURCE_DESC GPUResource::GetResourceDesc() const {
    D3D12_RESOURCE_DESC resourceDesc{};
    if (resource_) {
        resourceDesc = resource_->GetDesc();
    }
    return resourceDesc;
}

void GPUResource::SetResource(Microsoft::WRL::ComPtr<ID3D12Resource> resource, const D3D12_CLEAR_VALUE* clearValue = nullptr) {
    resource_ = resource;
    if (clearValue) {
        clearValue_ = std::make_unique<D3D12_CLEAR_VALUE>(clearValue);
    }
    else {
        clearValue_.reset();
    }
}

void GPUResource::SetName(const std::wstring& name) {
#ifdef _DEBUG
    name_ = name;
    if (resource_ && !name_.empty()) {
        resource_->SetName(name_.c_str());
    }
#else
    (void)name;
#endif
}

void GPUResource::Create(const D3D12_RESOURCE_DESC& resourceDesc, const D3D12_CLEAR_VALUE* clearValue, const std::wstring& name) {
    auto device = Graphics::Get().GetDevice();

    if (clearValue) {
        clearValue_ = std::make_unique<D3D12_CLEAR_VALUE>(*clearValue);
    }

    ASSERT_IF_FAILED(device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
        D3D12_HEAP_FLAG_NONE,
        &resourceDesc,
        D3D12_RESOURCE_STATE_COMMON,
        clearValue_.get(),
        IID_PPV_ARGS(resource_.GetAddressOf())));

    ResourceStateTracker::AddGlobalResourceState(resource_.Get(), D3D12_RESOURCE_STATE_COMMON);

    SetName(name);
}

void GPUResource::Reset() {
    resource_.Reset();
    clearValue_.reset();
}