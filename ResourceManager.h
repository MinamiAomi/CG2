#pragma once

#include <d3d12.h>
#include <wrl.h>

#include <cstdint>
#include <string>
#include <vector>

#include <memory>

#include "CommandList.h"

struct Descriptor {
    D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle;
    D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle;
};

class ResourceManager {
public:
    ResourceManager();

    void Initialize(Microsoft::WRL::ComPtr<ID3D12Device> device, const std::shared_ptr<CommandList>& commandList, uint32_t descriptorMaxCount);

    uint32_t LoadTexture(const std::string& name);

    template<class T>
    Microsoft::WRL::ComPtr<ID3D12Resource> CreateVertexResource(const std::vector<T>& vertices) {
        size_t bufferSize = sizeof(vertices[0]) * vertices.size();
        Microsoft::WRL::ComPtr<ID3D12Resource> resource = CreateBufferResource(bufferSize);
        T* mappedPtr = nullptr;
        resource->Map(0, nullptr, reinterpret_cast<void**>(&mappedPtr));
        memcpy(mappedPtr, vertices.data(), bufferSize);
        resource->Unmap(0, nullptr);
        return resource;
    }
    template<class T>
    Microsoft::WRL::ComPtr<ID3D12Resource> CreateIndexResource(const std::vector<T>& indices) {
        size_t bufferSize = sizeof(indices[0]) * indices.size();
        Microsoft::WRL::ComPtr<ID3D12Resource> resource = CreateBufferResource(bufferSize);
        T* mappedPtr = nullptr;
        resource->Map(0, nullptr, reinterpret_cast<void**>(&mappedPtr));
        memcpy(mappedPtr, indices.data(), bufferSize);
        resource->Unmap(0, nullptr);
        return resource;
    }
    template<class T>
    Microsoft::WRL::ComPtr<ID3D12Resource> CreateConstantResource(const T& constantData) {
        size_t bufferSize = sizeof(constantData);
        Microsoft::WRL::ComPtr<ID3D12Resource> resource = CreateBufferResource(bufferSize);
        T* mappedPtr = nullptr;
        resource->Map(0, nullptr, reinterpret_cast<void**>(&mappedPtr));
        memcpy(mappedPtr, &constantData, bufferSize);
        resource->Unmap(0, nullptr);
        return resource;
    }

    Microsoft::WRL::ComPtr<ID3D12Device> GetDevice() const { return device_; }
    std::shared_ptr<CommandList> GetCommandList() const { return commandList_; }
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> GetDescriptorHeap() const { return descriptorHeap_; }

private:
    Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(size_t bufferSize);
    void InternalLoadTexture(const std::string& name, size_t index);

    struct Texture {
        std::string name;
        Microsoft::WRL::ComPtr<ID3D12Resource> resource;
        D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle;
        D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle;
    };

    Microsoft::WRL::ComPtr<ID3D12Device> device_;
    std::shared_ptr<CommandList> commandList_;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap_;
    uint32_t descriptorMaxCount_;
    uint32_t descriptorAllocationIndex_;
    uint32_t descriptorSize_;

    std::vector<Texture> textures_;
};
