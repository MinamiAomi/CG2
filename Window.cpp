#include "Window.h"

#include <cassert>

#include "Utils.h"

#ifdef _DEBUG
#include "Externals/ImGui/imgui_impl_win32.h"
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif // _DEBUG

namespace CG {

    // ウィンドウプロシージャ
    LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {

#ifdef _DEBUG
        if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) { return true; }
#endif // _DEBUG

            // メッセージに対してゲーム固有の処理を行う
            switch (msg) {
            case WM_DESTROY:
                PostQuitMessage(0);
                return 0;
            }
        return DefWindowProc(hwnd, msg, wparam, lparam);
    }

    void CG::Window::Initialize(const std::string& name, uint32_t clientWidth, uint32_t clientHeight) {
        if (FAILED(CoInitializeEx(0, COINIT_MULTITHREADED))) {
            assert(false);
        }

        // ウィンドウクラスを生成
        WNDCLASS wc{};
        wc.lpfnWndProc = WindowProc;	// ウィンドウプロシージャ
        wc.lpszClassName = L"CGWindowClass";	// ウィンドウクラス名
        wc.hInstance = GetModuleHandle(nullptr);	// インスタンスハンドル
        wc.hCursor = LoadCursor(nullptr, IDC_ARROW);	// カーソル
        RegisterClass(&wc);	// ウィンドウクラスを登録

        // ウィンドウサイズを表す構造体にクライアント領域を入れる
        RECT wrc{ 0,0,static_cast<LONG>(clientWidth),static_cast<LONG>(clientHeight) };
        // クライアント領域を元に実際のサイズにwrcを変更してもらう
        AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

        auto wname = ConvertString(name);

        // ウィンドウの生成
        hWnd_ = CreateWindow(
            wc.lpszClassName,		// 利用するクラス名
            wname.c_str(),				// タイトルバーの文字
            WS_OVERLAPPEDWINDOW,	// よく見るウィンドウスタイル
            CW_USEDEFAULT,			// 表示X座標（WindowsOSに任せる）
            CW_USEDEFAULT,			// 表示Y座標（WindowsOSに任せる）
            wrc.right - wrc.left,	// ウィンドウ横幅
            wrc.bottom - wrc.top,	// ウィンドウ縦幅
            nullptr,				// 親ウィンドウハンドル
            nullptr,				// メニューハンドル
            wc.hInstance,			// インスタンスハンドル
            nullptr);				// オプション
        name_ = name;
        clientWidth_ = clientWidth;
        clientHeight_ = clientHeight;
    }

    void Window::Show() {
        ShowWindow(hWnd_, SW_SHOW);
    }

    bool Window::ProcessMessage() const {
        MSG msg{};

        while (msg.message != WM_QUIT) {
            if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
            else {
                break;
            }
        }
        return msg.message != WM_QUIT;
    }

    void Window::Finalize() {
        CloseWindow(hWnd_);
        CoUninitialize();
    }

}