#include "ImGuiManager.h"

#include "Externals/ImGui/imgui_impl_win32.h"
#include "Externals/ImGui/imgui_impl_dx12.h"

#include "Window.h"
#include "GraphicsEngine.h"

namespace CG {
    ImGuiManager* ImGuiManager::GetInstance() {
        static ImGuiManager instance;
        return &instance;
    }
    void CG::ImGuiManager::Initialize(Window* window, GraphicsEngine* graphicsEngine) {
        descriptor_ = graphicsEngine->GetSRVDescriptorHeap().Allocate();
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::StyleColorsDark();
        ImGui_ImplWin32_Init(window->GetHWND());
        ImGui_ImplDX12_Init(
            graphicsEngine->GetDevice().GetDevice().Get(),
            DX12::SwapChain::kBackBufferCount,
            graphicsEngine->GetSwapChain().GetRTVFormat(),
            graphicsEngine->GetSRVDescriptorHeap().GetDescriptorHeap().Get(),
            descriptor_.GetCPUHandle(),
            descriptor_.GetGPUHandle());
    }

    void ImGuiManager::NewFrame() {
        ImGui_ImplWin32_NewFrame();
        ImGui_ImplDX12_NewFrame();
        ImGui::NewFrame();
    }

    void ImGuiManager::Render(DX12::CommandList& commandList) {
        ImGui::Render();
        ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList.GetCommandList().Get());
    }

    void ImGuiManager::Finalize() {
        ImGui_ImplDX12_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
    }

}