#pragma once

#include "DX12/DX12.h"

namespace CG {

    class Window;
    class GraphicsEngine;

    class GUI {
    public:
        static LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

        void Initialize(const Window& window, GraphicsEngine& device);
        void NewFrame();
        void Render(const DX12::CommandList& commandList);
        void Finalize();

    private:
        DX12::Descriptor descriptor_;
    };

}