#include "Texture.h"

#include <cassert>

#include "../Externals/DirectXTex/DirectXTex.h"
#include "../Externals/DirectXTex/d3dx12.h"

#include "Device.h"
#include "CommandQueue.h"
#include "CommandList.h"
#include "Fence.h"

namespace CG::DX12 {

    void Texture::InitializeFromPNG(const Device& device, CommandQueue& commandQueue, const std::wstring& name) {
        assert(device.IsEnabled());
        assert(commandQueue.IsEnabled());

        DirectX::ScratchImage image{};
        if (FAILED(DirectX::LoadFromWICFile(name.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image))) {
            assert(false);
        }

        DirectX::ScratchImage mipImages{};
        if (FAILED(DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages))) {
            assert(false);
        }


        {
            const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
            D3D12_RESOURCE_DESC resourceDesc{};
            resourceDesc.Width = UINT(metadata.width);
            resourceDesc.Height = UINT(metadata.height);
            resourceDesc.MipLevels = UINT16(metadata.mipLevels);
            resourceDesc.DepthOrArraySize = UINT16(metadata.arraySize);
            resourceDesc.Format = metadata.format;
            resourceDesc.SampleDesc.Count = 1;
            resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension);

            D3D12_HEAP_PROPERTIES heapProperties{};
            heapProperties.Type = D3D12_HEAP_TYPE_CUSTOM;
            heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
            heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;

            ComPtr<ID3D12Resource> resource;
            if (FAILED(device.GetDevice()->CreateCommittedResource(
                &heapProperties,
                D3D12_HEAP_FLAG_NONE,
                &resourceDesc,
                D3D12_RESOURCE_STATE_COPY_DEST,
                nullptr,
                IID_PPV_ARGS(resource.GetAddressOf())))) {
                assert(false);
            }

            resource_.InitializeForResource(resource, Resource::State::CopyDest);
        }

        std::vector<D3D12_SUBRESOURCE_DATA> subresources;
        DirectX::PrepareUpload(device.GetDevice().Get(), mipImages.GetImages(), mipImages.GetImageCount(), mipImages.GetMetadata(), subresources);
        uint64_t intermediateSize = GetRequiredIntermediateSize(resource_.GetResource().Get(), 0, UINT(subresources.size()));
        Resource intermediateResource;
        intermediateResource.InitializeForBuffer(device, intermediateSize);

        CommandList commandList;
        commandList.Initialize(device, commandQueue);
        commandList.Reset();
        Fence fence;
        fence.Initialize(device);

        UpdateSubresources(commandList.GetCommandList().Get(), resource_.GetResource().Get(), intermediateResource.GetResource().Get(), 0, 0, UINT(subresources.size()), subresources.data());
        D3D12_RESOURCE_BARRIER barrier = resource_.TransitionBarrier(Resource::State::GenericRead);
        barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        commandList.GetCommandList()->ResourceBarrier(1, &barrier);
        commandList.Close();
        commandQueue.ExcuteCommandList(commandList);
        fence.Signal(commandQueue);
        fence.Wait();

        desc_ = resource_.GetResource()->GetDesc();
    }

}