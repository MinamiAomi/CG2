#include "GUI.h"

#include "Externals/ImGui/imgui.h"
#include "Externals/ImGui/imgui_impl_win32.h"
#include "Externals/ImGui/imgui_impl_dx12.h"

#include "Window.h"
#include "GraphicsEngine.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace CG {
    
    LRESULT GUI::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
        return ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam);
    }

    void GUI::Initialize(const Window& window, GraphicsEngine& device) {
        descriptor_ = device.GetSRVDescriptorHeap().Allocate();

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::StyleColorsDark();
        ImGui_ImplWin32_Init(window.GetHWND());
        ImGui_ImplDX12_Init(
            device.GetDevice().GetDevice().Get(),
            DX12::SwapChain::kBackBufferCount,
            device.GetSwapChain().GetCorrentRenderTargetView().GetDesc().Format,
            device.GetSRVDescriptorHeap().GetDescriptorHeap().Get(),
            descriptor_.GetCPUHandle(),
            descriptor_.GetGPUHandle());
    }

    void GUI::NewFrame() {
        ImGui_ImplWin32_NewFrame();
        ImGui_ImplDX12_NewFrame();
        ImGui::NewFrame();
    }

    void GUI::Render(const DX12::CommandList& commandList) {
        auto cmdList = commandList.GetCommandList();
        ID3D12DescriptorHeap* ppHeaps[] = { descriptor_.GetHeap()->GetDescriptorHeap().Get() };
        cmdList->SetDescriptorHeaps(1, ppHeaps);
        ImGui::Render();
        ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), cmdList.Get());
    }

    void GUI::Finalize() {
         ImGui_ImplDX12_Shutdown();
         ImGui_ImplWin32_Shutdown();
         ImGui::DestroyContext();
    }

}