#include "Input.h"

#include <wrl.h>

#include <cassert>
#include <cstdint>
#include <cstdlib>

#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

namespace CG {

    class Input::Impl {
    public:
        static constexpr uint32_t kKeyCount = 256;

        struct MouseState {
            DIMOUSESTATE state;
            Vector2 position;
        };

        void Initialize(HWND hwnd) {
            directInput_.Reset();
            keybord_.Reset();

            if (FAILED(DirectInput8Create(
                GetModuleHandle(nullptr), DIRECTINPUT_HEADER_VERSION,
                IID_IDirectInput8, (void**)directInput_.GetAddressOf(), nullptr))) {
                assert(false);
            }

            if (FAILED((directInput_->CreateDevice(GUID_SysKeyboard, &keybord_, nullptr)))) {
                assert(false);
            }
            if (FAILED((keybord_->SetDataFormat(&c_dfDIKeyboard)))) {
                assert(false);
            }
            if (FAILED((keybord_->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY)))) {
                assert(false);
            }

            if (FAILED(directInput_->CreateDevice(GUID_SysMouse, &mouse_, nullptr))) {
                assert(false);
            }
            if (FAILED(mouse_->SetDataFormat(&c_dfDIMouse))) {
                assert(false);
            }
            if (FAILED(mouse_->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY))) {
                assert(false);
            }
            hwnd_ = hwnd;
        }
        void Update() {
            memcpy(preKeys_, keys_, sizeof(keys_));
            keybord_->Acquire();
            keybord_->GetDeviceState(sizeof(keys_), keys_);

            preMouseState_ = mouseState_;
            mouse_->Acquire();
            mouse_->GetDeviceState(sizeof(mouseState_.state), &mouseState_.state);

            POINT p{};
            GetCursorPos(&p);
            ScreenToClient(hwnd_, &p);
            mouseState_.position = { static_cast<float>(p.x), static_cast<float>(p.y) };

        }

        uint8_t keys_[kKeyCount]{};
        uint8_t preKeys_[kKeyCount]{};
        MouseState mouseState_{};
        MouseState preMouseState_{};
    private:

        template<class T>
        using ComPtr = Microsoft::WRL::ComPtr<T>;

        HWND hwnd_{ nullptr };
        ComPtr<IDirectInput8> directInput_;
        ComPtr<IDirectInputDevice8> keybord_;
        ComPtr<IDirectInputDevice8> mouse_;

    };

    void Input::Initialize(HWND hwnd) {
        if (!hwnd) {
            hwnd = GetActiveWindow();
        }
        GetInstance()->pimpl_->Initialize(hwnd);
    }

    void Input::Update() {
        GetInstance()->pimpl_->Update();
    }

    bool Input::IsKeyTriggered(Keycode keycode) {
        auto& k = *GetInstance()->pimpl_;
        return  k.keys_[static_cast<unsigned char>(keycode)] != 0 &&
            k.preKeys_[static_cast<unsigned char>(keycode)] == 0;
    }

    bool Input::IsKeyPressed(Keycode keycode) {
        auto& k = *GetInstance()->pimpl_;
        return  k.keys_[static_cast<unsigned char>(keycode)] != 0;
    }

    bool Input::IsKeyReleaseed(Keycode keycode) {
        auto& k = *GetInstance()->pimpl_;
        return  k.keys_[static_cast<unsigned char>(keycode)] == 0 &&
            k.preKeys_[static_cast<unsigned char>(keycode)] != 0;
    }

    bool Input::IsMouseTriggered(MouseButton button) {
        auto& k = *GetInstance()->pimpl_;
        return  k.mouseState_.state.rgbButtons[static_cast<size_t>(button)] & 0x80 &&
            !(k.preMouseState_.state.rgbButtons[static_cast<size_t>(button)] & 0x80);
    }

    bool Input::IsMousePressed(MouseButton button) {
        auto& k = *GetInstance()->pimpl_;
        return  k.mouseState_.state.rgbButtons[static_cast<size_t>(button)] & 0x80;
    }

    bool Input::IsMouseReleaseed(MouseButton button) {
        auto& k = *GetInstance()->pimpl_;
        return  !(k.mouseState_.state.rgbButtons[static_cast<size_t>(button)] & 0x80) &&
            k.preMouseState_.state.rgbButtons[static_cast<size_t>(button)] & 0x80;
    }

    const Vector2& Input::GetMousePosition() {
        auto& k = *GetInstance()->pimpl_;
        return  k.mouseState_.position;
    }

    Vector2 Input::GetMouseMove() {
        auto& k = *GetInstance()->pimpl_;
        return { static_cast<float>(k.mouseState_.state.lX),static_cast<float>(k.mouseState_.state.lY) };
    }

    float Input::GetWheel() {
        auto& k = *GetInstance()->pimpl_;
        return static_cast<float>(k.mouseState_.state.lZ);
    }

    Input* Input::GetInstance() {
        static Input instance;
        return &instance;
    }

    Input::Input() {
        pimpl_ = new Impl;
    }

    Input::~Input() {
        delete pimpl_;
    }

}