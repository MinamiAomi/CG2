#pragma once
#include <Windows.h>

#include <cstdint>
#include <string>

namespace CG {

    class Window {
    public:
        static Window* GetInstance();

        void Initialize(const std::string& name, uint32_t clientWidth, uint32_t clientHeight);
        void Show();
        bool ProcessMessage() const;
        void Finalize();

        bool IsEnabled() { return hWnd_; }
        const std::string& GetName() const { return name_; }
        HWND GetHWND() const { return hWnd_; }
        uint32_t GetClientWidth() const { return clientWidth_; }
        uint32_t GetClientHeight() const { return clientHeight_; }

    private:
        Window() = default;
        Window(const Window&) = delete;
        const Window& operator=(const Window&) = delete;

        std::string name_;
        HWND hWnd_{ nullptr };
        uint32_t clientWidth_{ 0 };
        uint32_t clientHeight_{ 0 };
    };

}