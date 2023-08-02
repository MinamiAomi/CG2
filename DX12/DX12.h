#pragma once

#include <d3d12.h>
#include <dxgidebug.h>
#include <wrl/client.h>

#include <cstdint>

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
#include "Texture.h"

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

    struct Viewport : public D3D12_VIEWPORT {
        Viewport() = default;
        explicit Viewport(const D3D12_VIEWPORT& viewport) noexcept : D3D12_VIEWPORT(viewport) {}
        explicit Viewport(
            float topLeftX,
            float topLeftY,
            float width,
            float height,
            float minDepth = D3D12_MIN_DEPTH,
            float maxDepth = D3D12_MAX_DEPTH) noexcept {
            TopLeftX = topLeftX;
            TopLeftY = topLeftY;
            Width = width;
            Height = height;
            MinDepth = minDepth;
            MaxDepth = maxDepth;
        }
        explicit Viewport(float width, float height) noexcept : Viewport(0.0f, 0.0f, width, height) {}
    };

    struct ScissorRect : public D3D12_RECT {
        ScissorRect() = default;
        explicit ScissorRect(const D3D12_RECT& rect) noexcept : D3D12_RECT(rect) {}
        explicit ScissorRect(
            LONG Left,
            LONG Top,
            LONG Right,
            LONG Bottom) noexcept {
            left = Left;
            top = Top;
            right = Right;
            bottom = Bottom;
        }
        explicit ScissorRect(const Viewport& viewport) :
            ScissorRect(
                static_cast<LONG>(viewport.TopLeftX),
                static_cast<LONG>(viewport.TopLeftY),
                static_cast<LONG>(viewport.TopLeftX + viewport.Width),
                static_cast<LONG>(viewport.TopLeftY + viewport.Height)) {

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

    struct VertexBuffer {
        Resource resource;
        VertexBufferView view;
    };

    struct IndexBuffer {
        Resource resource;
        IndexBufferView view;
    };

    struct RenderTargetResource {
        Resource resource;
        RenderTargetView view;
    };

    class DepthStencilResource {
    public:
        void Initialize(const Device& device, DescriptorHeap& descriptorHeap, uint32_t width, uint32_t height);
        
        Resource& GetResource() { return resource_; }
        const Resource& GetResource() const { return resource_; }
        D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle() const { return view_.GetDescriptor().GetCPUHandle(); }
        const DepthStencilView& GetView() const { return view_; }

    private:
        Resource resource_;
        DepthStencilView view_;
    };

}
