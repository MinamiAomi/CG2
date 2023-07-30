#pragma once

#include <dinput.h>

#include "Math/MathUtils.h"

enum class Keycode : unsigned char {
    Esc = 0x01,	/* Escape */
    x1 = 0x02,	// 1
    x2 = 0x03,	// 2
    x3 = 0x04,	// 3
    x4 = 0x05,	// 4
    x5 = 0x06,	// 5
    x6 = 0x07,	// 6
    x7 = 0x08,	// 7
    x8 = 0x09,	// 8
    x9 = 0x0A,	// 9
    x0 = 0x0B,	// 0
    Minus = 0x0C,	// - (マイナス)
    Equals = 0x0D,	// = (イコール)
    BackSpace = 0x0E,	// BackSpace
    Tab = 0x0F,	// Tab
    Q = 0x10,	// Q
    W = 0x11,	// W
    E = 0x12,	// E
    R = 0x13,	// R
    T = 0x14,	// T
    Y = 0x15,	// Y
    U = 0x16,	// U
    I = 0x17,	// I
    O = 0x18,	// O
    P = 0x19,	// P
    LBracket = 0x1A,	// [ (左ブラケット)
    RBracket = 0x1B,	// ] (右ブラケット)
    Enter = 0x1C,	// Enter
    LCtrl = 0x1D,	// Left Ctrl (左コントロール)
    A = 0x1E,	// A	
    S = 0x1F,	// S
    D = 0x20,	// D
    F = 0x21,	// F
    G = 0x22,	// G
    H = 0x23,	// H
    J = 0x24,	// J
    K = 0x25,	// K
    L = 0x26,	// L
    Semicolon = 0x27,	// ; (セミコロン)
    Apostrophe = 0x28,	// ' (シングルクォーテーション)
    Grave = 0x29,	// ` (グライヴ)
    LShift = 0x2A,	// Left Shift (左シフト)
    BackSlash = 0x2B,	// \ (バックスラッシュ)
    Z = 0x2C,	// Z 
    X = 0x2D,	// X 
    C = 0x2E,	// C
    V = 0x2F,	// V
    B = 0x30,	// B
    N = 0x31,	// N
    M = 0x32,	// M
    Comma = 0x33,	// , (カンマ)
    Period = 0x34,	// . (ピリオド)
    Slash = 0x35,	// / (スラッシュ)
    RShift = 0x36,	// Right Shift (右シフト)
    NumPadMultiply = 0x37,
    LAlt = 0x38,	// Left Alt (左オルト)
    Space = 0x39,	// Space (スペース)
    CapsLock = 0x3A,
    F1 = 0x3B,	// F1 
    F2 = 0x3C,	// F2 
    F3 = 0x3D,	// F3 
    F4 = 0x3E,	// F4 
    F5 = 0x3F,	// F5 
    F6 = 0x40,	// F6 
    F7 = 0x41,	// F7 
    F8 = 0x42,	// F8 
    F9 = 0x43,	// F9 
    F10 = 0x44,	// F10
    NumLock = 0x45,
    ScrollLock = 0x46,
    NumPad7 = 0x47,
    NumPad8 = 0x48,
    NumPad9 = 0x49,
    NumPadSubtract = 0x4A,
    NumPad4 = 0x4B,
    NumPad5 = 0x4C,
    NumPad6 = 0x4D,
    NumPadAdd = 0x4E,
    NumPad1 = 0x4F,
    NumPad2 = 0x50,
    NumPad3 = 0x51,
    NumPad0 = 0x52,
    NumPadDecimal = 0x53,
    F11 = 0x57,
    F12 = 0x58,
    F13 = 0x64,
    F14 = 0x65,
    F15 = 0x66,
    Kana = 0x70,
    Convert = 0x79,
    NoConvert = 0x7B,
    Yen = 0x7D,
    NumPadEquals = 0x8D,
    PrevTrack = 0x90,
    At = 0x91,	// @ (アットマーク)
    Colon = 0x92,	// : (コロン)
    UnderLine = 0x93,	// _ (アンダーライン)
    Kanji = 0x94,
    Stop = 0x95,
    AX = 0x96,
    Unlabeled = 0x97,
    NextTrack = 0x99,
    NumPadEnter = 0x9C,
    RCtrl = 0x9D,	// Right Ctrl (右コントロール)
    Mute = 0xA0,
    Calculator = 0xA1,
    PlayPause = 0xA2,
    MediaStop = 0xA4,
    VolumeDown = 0xAE,
    VolumeUp = 0xB0,
    WebHome = 0xB2,
    NumPadComma = 0xB3,
    NumPadDivide = 0xB5,
    SysRq = 0xB7,
    RAlt = 0xB8,
    Pause = 0xC5,
    Home = 0xC7,
    Up = 0xC8,	// ↑ (上矢印)
    PageUp = 0xC9,
    Left = 0xCB,	// ← (左矢印)
    Right = 0xCD,	// → (右矢印)
    End = 0xCF,
    Down = 0xD0,	// ↓ (下矢印)
    PageDown = 0xD1,
    Insert = 0xD2,
    Delete = 0xD3,	// Delete (デリート)
    LWindows = 0xDB,
    RWindows = 0xDC,
    Menu = 0xDD,
    Power = 0xDE,
    Sleep = 0xDF,
};

enum class MouseButton {
    Left,
    Right,
    Mid
};

class Input {
public:
    static void Initialize(HWND hwnd = nullptr);
    static void Update();
    static bool IsKeyTriggered(Keycode keycode);
    static bool IsKeyPressed(Keycode keycode);
    static bool IsKeyReleaseed(Keycode keycode);
    static bool IsMouseTriggered(MouseButton button);
    static bool IsMousePressed(MouseButton button);
    static bool IsMouseReleaseed(MouseButton button);
    static const Vector2& GetMousePosition();
    static Vector2 GetMouseMove();
    static float GetWheel();

private:
    static Input* GetInstance();

    Input();
    ~Input();

    class Impl; Impl* pimpl_;
};