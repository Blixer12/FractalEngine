#pragma once

#include "Defines.h"

typedef enum MouseButtons {
    MouseButtonLeft,
    MouseButtonRight,
    MouseButtonMiddle,

    MouseButtonForward,
    MouseButtonBackwards,

    MouseButtonScrollUp,
    MouseButtonScrollDown,

    ButtonCount
} MouseButtons;

#define DefineKey(Name, Code) Name##Key = Code

typedef enum Keys {
    DefineKey(Backspace, 0x08),
    DefineKey(Enter, 0x0D),
    DefineKey(Tab, 0x09),
    DefineKey(Shift, 0x10),
    DefineKey(Control, 0x11),

    DefineKey(Pause, 0x13),
    DefineKey(Capital, 0x14),

    DefineKey(Escape, 0x1B),

    DefineKey(Convert, 0x1C),
    DefineKey(NonConvert, 0x1D),
    DefineKey(Accept, 0x1E),
    DefineKey(ModeChange, 0x1F),

    DefineKey(Space, 0x20),
    DefineKey(Prior, 0x21),
    DefineKey(Next, 0x22),
    DefineKey(End, 0x23),
    DefineKey(Home, 0x24),
    DefineKey(Left, 0x25),
    DefineKey(Up, 0x26),
    DefineKey(Right, 0x27),
    DefineKey(Down, 0x28),
    DefineKey(Select, 0x29),
    DefineKey(Print, 0x2A),
    DefineKey(Execute, 0x2B),
    DefineKey(Snapshot, 0x2C),
    DefineKey(Insert, 0x2D),
    DefineKey(Delete, 0x2E),
    DefineKey(Help, 0x2F),

    // --- MAIN NUMBER KEYS (0-9 Above Letters) ---
    DefineKey(Num0, 0x30),
    DefineKey(Num1, 0x31),
    DefineKey(Num2, 0x32),
    DefineKey(Num3, 0x33),
    DefineKey(Num4, 0x34),
    DefineKey(Num5, 0x35),
    DefineKey(Num6, 0x36),
    DefineKey(Num7, 0x37),
    DefineKey(Num8, 0x38),
    DefineKey(Num9, 0x39),

    DefineKey(A, 0x41),
    DefineKey(B, 0x42),
    DefineKey(C, 0x43),
    DefineKey(D, 0x44),
    DefineKey(E, 0x45),
    DefineKey(F, 0x46),
    DefineKey(G, 0x47),
    DefineKey(H, 0x48),
    DefineKey(I, 0x49),
    DefineKey(J, 0x4A),
    DefineKey(K, 0x4B),
    DefineKey(L, 0x4C),
    DefineKey(M, 0x4D),
    DefineKey(N, 0x4E),
    DefineKey(O, 0x4F),
    DefineKey(P, 0x50),
    DefineKey(Q, 0x51),
    DefineKey(R, 0x52),
    DefineKey(S, 0x53),
    DefineKey(T, 0x54),
    DefineKey(U, 0x55),
    DefineKey(V, 0x56),
    DefineKey(W, 0x57),
    DefineKey(X, 0x58),
    DefineKey(Y, 0x59),
    DefineKey(Z, 0x5A),

    DefineKey(LWin, 0x5B),
    DefineKey(RWin, 0x5C),
    DefineKey(Apps, 0x5D),

    DefineKey(Sleep, 0x5F),

    // --- NUMERIC KEYPAD ---
    DefineKey(Numpad0, 0x60),
    DefineKey(Numpad1, 0x61),
    DefineKey(Numpad2, 0x62),
    DefineKey(Numpad3, 0x63),
    DefineKey(Numpad4, 0x64),
    DefineKey(Numpad5, 0x65),
    DefineKey(Numpad6, 0x66),
    DefineKey(Numpad7, 0x67),
    DefineKey(Numpad8, 0x68),
    DefineKey(Numpad9, 0x69),
    DefineKey(Multiply, 0x6A),
    DefineKey(Add, 0x6B),
    DefineKey(Separator, 0x6C),
    DefineKey(Subtract, 0x6D),
    DefineKey(Decimal, 0x6E),
    DefineKey(Divide, 0x6F),
    
    DefineKey(F1, 0x70),
    DefineKey(F2, 0x71),
    DefineKey(F3, 0x72),
    DefineKey(F4, 0x73),
    DefineKey(F5, 0x74),
    DefineKey(F6, 0x75),
    DefineKey(F7, 0x76),
    DefineKey(F8, 0x77),
    DefineKey(F9, 0x78),
    DefineKey(F10, 0x79),
    DefineKey(F11, 0x7A),
    DefineKey(F12, 0x7B),
    DefineKey(F13, 0x7C),
    DefineKey(F14, 0x7D),
    DefineKey(F15, 0x7E),
    DefineKey(F16, 0x7F),
    DefineKey(F17, 0x80),
    DefineKey(F18, 0x81),
    DefineKey(F19, 0x82),
    DefineKey(F20, 0x83),
    DefineKey(F21, 0x84),
    DefineKey(F22, 0x85),
    DefineKey(F23, 0x86),
    DefineKey(F24, 0x87),

    DefineKey(NumLock, 0x90),
    DefineKey(Scroll, 0x91),
    DefineKey(NumpadEqual, 0x92),

    DefineKey(LShift, 0xA0),
    DefineKey(RShift, 0xA1),
    DefineKey(LControl, 0xA2),
    DefineKey(RControl, 0xA3),
    DefineKey(LMenu, 0xA4),
    DefineKey(RMenu, 0xA5),

    // --- PUNCTUATION AND SYMBOLS ---
    DefineKey(Semicolon, 0xBA),  // ; or :
    DefineKey(Plus, 0xBB),       // = or +
    DefineKey(Comma, 0xBC),      // , or <
    DefineKey(Minus, 0xBD),      // - or _
    DefineKey(Period, 0xBE),     // . or >
    DefineKey(Slash, 0xBF),      // / or ?
    DefineKey(Grave, 0xC0),      // ` or ~
    
    DefineKey(LBracket, 0xDB),   // [ or {
    DefineKey(Backslash, 0xDC),  // \ or |
    DefineKey(RBracket, 0xDD),   // ] or }
    DefineKey(Quote, 0xDE),      // ' or "

    KeysCount
} Keys;

void InputSystemInitialize();
void InputSystemShutdown();
void InputUpdate(Float64 DeltaTime);

// --- KEYBOARD INPUT ---
FAPI Bool8 InputIsKeyDown(Keys Key);
FAPI Bool8 InputIsKeyUp(Keys Key);
FAPI Bool8 InputWasKeyDown(Keys Key);
FAPI Bool8 InputWasKeyUp(Keys Key);

void InputProccessKey(Keys Key, Bool8 Pressed);

// --- MOUSE INPUT ---
FAPI Bool8 InputIsMouseButtonDown(MouseButtons Button);
FAPI Bool8 InputIsMouseButtonUp(MouseButtons Button);
FAPI Bool8 InputWasMouseButtonDown(MouseButtons Button);
FAPI Bool8 InputWasMouseButtonUp(MouseButtons Button);
FAPI void InputGetMousePosition(Int32 * X, Int32 * Y);
FAPI void InputGetPreviousMousePosition(Int32* X, Int32* Y);

void InputProcessMouseButton(MouseButtons Button, Bool8 Pressed);
void InputProcessMouseMove(Int16 X, Int16 Y);
void InputProcessMouseWheel(Int8 WheelDelta);