#pragma once

#include "Externals/ImGui/imgui.h"

#include "DX12/DX12.h"

namespace CG {

    class Window;
    class GraphicsEngine;

    class ImGuiManager {
    public:
        static ImGuiManager* GetInstance();

        void Initialize(Window* window, GraphicsEngine* graphicsEngine);
        void NewFrame();
        void Render(DX12::CommandList& commandList);
        void Finalize();

    private:
        ImGuiManager() = default;
        ImGuiManager(const ImGuiManager&) = delete;
        const ImGuiManager& operator=(const ImGuiManager&) = delete;

        DX12::Descriptor descriptor_;
    };

}