#pragma once

#include <d3d12.h>
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

}