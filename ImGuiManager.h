#pragma once

#include "Externals/ImGui/imgui.h"

#include "DX12/DX12.h"

namespace CG {

    class Window;
    class GraphicsEngine;

    class ImGuiManager {
    public:
        void Initialize(const Window& window, GraphicsEngine& graphicsEngine);
        void NewFrame();
        void Render(DX12::CommandList& commandList);
        void Finalize();

    private:
        DX12::Descriptor descriptor_;
    };

}