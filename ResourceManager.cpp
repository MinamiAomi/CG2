#include "ResourceManager.h"

#include <cassert>
#include "DirectXUtils.h"

ResourceManager::ResourceManager() :
    descriptorMaxCount_(0),
    descriptorAllocationIndex_(0) {
}

void ResourceManager::Initialize(Microsoft::WRL::ComPtr<ID3D12Device> device, const std::shared_ptr<CommandList>& commandList, uint32_t descriptorMaxCount) {
    assert(device);
    assert(commandList);
    device_ = device;
    commandList_ = commandList;

    descriptorSize_ = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    descriptorMaxCount_ = descriptorMaxCount;

    descriptorHeap_ = CreateDescriptorHeap(device_, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, descriptorMaxCount_, true);
    descriptorAllocationIndex_ = 0;

    textures_.resize(descriptorMaxCount_);
    for (auto& texture : textures_) {
        texture.cpuHandle = { 0 };
        texture.gpuHandle = { 0 };
        texture.resource.Reset();
    }
}

uint32_t ResourceManager::LoadTexture(const std::string& name) {

    // 読み込み済みのテクスチャか検索
    auto texturesEnd = textures_.begin() + descriptorAllocationIndex_;
    auto iter = std::find_if(textures_.begin(), texturesEnd,
        [&](const Texture& texture) {
            if (texture.resource && texture.name == name) {
                return true;
            }
            return false;
        });
    // 読み込み済み場合
    if (iter != texturesEnd) {
        return static_cast<uint32_t>(std::distance(textures_.begin(), iter));
    }

    InternalLoadTexture(name, descriptorAllocationIndex_);

    uint32_t handle = descriptorAllocationIndex_;
    ++descriptorAllocationIndex_;
    return handle;
}

Microsoft::WRL::ComPtr<ID3D12Resource> ResourceManager::CreateBufferResource(size_t bufferSize) {
    // アップロードヒープ
    D3D12_HEAP_PROPERTIES uploadHeapProperties{};
    uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
    // 頂点バッファの設定
    D3D12_RESOURCE_DESC bufferDesc{};
    bufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    bufferDesc.Width = bufferSize;
    bufferDesc.Height = 1;
    bufferDesc.DepthOrArraySize = 1;
    bufferDesc.MipLevels = 1;
    bufferDesc.SampleDesc.Count = 1;
    bufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    Microsoft::WRL::ComPtr<ID3D12Resource> result = nullptr;
    // 頂点バッファを生成
    HRESULT hr = S_FALSE;
    hr = device_->CreateCommittedResource(
        &uploadHeapProperties,
        D3D12_HEAP_FLAG_NONE,
        &bufferDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(result.GetAddressOf()));
    assert(SUCCEEDED(hr));
    return result;
}

void ResourceManager::InternalLoadTexture(const std::string& name, size_t index) {
    auto& texture = textures_[index];

    texture.name = name;

    auto mipImages = ::LoadTexture("resources/monsterBall.png");
    const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
    texture.resource = CreateTextureResource(device_, metadata);

    Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource = UploadTextureData(texture.resource, mipImages, device_, commandList_->Get());
    commandList_->ExcuteCommand();
    commandList_->WaitForGPU();
    commandList_->Reset();
    
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Format = metadata.format;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);

    texture.cpuHandle = GetCPUDescriptorHandle(descriptorHeap_, descriptorSize_, uint32_t(index));
    texture.gpuHandle = GetGPUDescriptorHandle(descriptorHeap_, descriptorSize_, uint32_t(index));

    device_->CreateShaderResourceView(texture.resource.Get(), &srvDesc, texture.cpuHandle);
}
