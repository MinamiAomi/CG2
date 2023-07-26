#pragma once

#include <d3d12.h>
#include <dxgidebug.h>
#include <wrl/client.h>

#include "Device.h"
#include "CommandQueue.h"
#include "SwapChain.h"
#include "CommandList.h"
#include "Fence.h"
#include "DescriptorHeap.h"
#include "Descriptor.h"
#include "Resource.h"
#include "Views.h"
#include "RootSignature.h"
#include "PipelineState.h"
#include "Shader.h"
#include "ShaderCompiler.h"

namespace CG::DX12 {

    class D3DResourceLeakChecker {
    public:
        ~D3DResourceLeakChecker() {
            Microsoft::WRL::ComPtr<IDXGIDebug1> debug = nullptr;
            if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(debug.GetAddressOf())))) {
                debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
                debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
                debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
            }
        }
    };

    class DynamicBuffer {
    public:
        void Initialize(const Device& device, size_t bufferSize, bool allowUnorderedAccess = false, CG::DX12::Resource::State initialState = CG::DX12::Resource::State::GenericRead);
        
        Resource& GetResource() { return resource_; }
        const Resource& GetResource() const { return resource_; }
        size_t GetBufferSize() const { return bufferSize_; }
        template<class T>
        T* GetDataBegin() const { return reinterpret_cast<T*>(dataBegin_); }
        void* GetDataBegin() const { return dataBegin_; }
    
    private:
        Resource resource_;
        size_t bufferSize_{ 0 };
        void* dataBegin_{ nullptr };
    };

    struct RenderTargetResource {
        Resource resource;
        RenderTargetView view;
    };

    struct DepthStencilResource {
        Resource resource;
        DepthStencilView view;
    };

    struct TextureResource {
        Resource resource;
        ShaderResourceView view;
    };
}