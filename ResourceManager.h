#pragma once

#include <d3d12.h>

#include <cstdint>
#include <string>
#include <vector>

struct Descriptor {
    D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle;
    D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle;
};

class ResourceManager {
public:
    ResourceManager();
    ~ResourceManager();

    void Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, uint32_t descriptorMaxCount);

    uint32_t LoadTexture(const std::string& name);

    template<class T>
    ID3D12Resource* CreateVertexResource(const std::vector<T>& vertices) {
        size_t bufferSize = sizeof(vertices[0]) * vertices.size();
        ID3D12Resource* resource = CreateBufferResource(bufferSize);
        T* mappedPtr = nullptr;
        resource->Map(0, nullptr, reinterpret_cast<void**>(&mappedPtr));
        memcpy(mappedPtr, vertices.data(), bufferSize);
        resource->Unmap(0, nullptr);
        return resource;
    }
    template<class T>
    ID3D12Resource* CreateIndexResource(const std::vector<T>& indices) {
        size_t bufferSize = sizeof(indices[0]) * indices.size();
        ID3D12Resource* resource = CreateBufferResource(bufferSize);
        T* mappedPtr = nullptr;
        resource->Map(0, nullptr, reinterpret_cast<void**>(&mappedPtr));
        memcpy(mappedPtr, indices.data(), bufferSize);
        resource->Unmap(0, nullptr);
        return resource;
    }
    template<class T>
    ID3D12Resource* CreateConstantResource(const T& constantData) {
        size_t bufferSize = sizeof(constantData);
        ID3D12Resource* resource = CreateBufferResource(bufferSize);
        T* mappedPtr = nullptr;
        resource->Map(0, nullptr, reinterpret_cast<void**>(&mappedPtr));
        memcpy(mappedPtr, &constantData, bufferSize);
        resource->Unmap(0, nullptr);
        return resource;
    }

    ID3D12Device* GetDevice() const { return device_; }
    ID3D12GraphicsCommandList* GetCommandList() const { return commandList_; }
    ID3D12DescriptorHeap* GetDescriptorHeap() const { return descriptorHeap_; }

private:
    ID3D12Resource* CreateBufferResource(size_t bufferSize);

    struct Texture {
        std::string name;
        ID3D12Resource* resource;
        D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle;
        D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle;
    };

    ID3D12Device* device_;
    ID3D12GraphicsCommandList* commandList_;
    ID3D12DescriptorHeap* descriptorHeap_;
    uint32_t descriptorMaxCount_;
    uint32_t descriptorAllocationIndex_;
    uint32_t descriptorSize_;

    std::vector<Texture> textures_;
};
