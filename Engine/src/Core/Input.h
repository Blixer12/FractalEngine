/**
 * @file Input.h
 * @author Blixer12
 * @brief This file contains everything having to do with input on deskop
 * environments from keyboards and mice. Gamepads and touch controls will
 * likely be handled separately at a future date.
 * 
 * @version 1.0
 * @date 2026-10-11
 * 
 * @copyright Fractal Engine is Copyright (c) Blixer12 2026
 * 
 */

#pragma once

#include "Defines.h"

/**
 * @brief Represents available mouse buttons.
 */
typedef enum MouseButtons {
    /** @brief The left mouse button */
    MouseButtonLeft,
    /** @brief The right mouse button */
    MouseButtonRight,
    /** @brief The middle mouse button (typically the wheel) */
    MouseButtonMiddle,

    /** @brief The forward mouse button (typically the arrow buttons on the side)*/
    MouseButtonForward,
    /** @brief The backwards mouse button (typically the arrow buttons on the side)*/
    MouseButtonBackwards,

    /** @brief The scroll forwards action (typically the same direction on mouse)*/
    MouseButtonScrollUp,
    /** @brief The scroll backwards action (typically the same direction on mouse)*/
    MouseButtonScrollDown,

    ButtonCount
} MouseButtons;

/**
 * @brief Represents available keyboard keys.
 */
typedef enum Keys {
    /** @brief The backspace key. */
    BackspaceKey = 0x08,
    /** @brief The enter key. */
    EnterKey = 0x0D,
    /** @brief The tab key. */
    TabKey = 0x09,
    /** @brief The shift key. */
    ShiftKey = 0x10,
    /** @brief The control key. */
    ControlKey = 0x11,

    /** @brief The pause key. */
    PauseKey = 0x13,
    /** @brief The caps lock key. */
    CapitalKey = 0x14,

    /** @brief The escape key. */
    EscapeKey = 0x1B,

    /** @brief The convert key. */
    ConvertKey = 0x1C,
    /** @brief The non-convert key. */
    NonConvertKey = 0x1D,
    /** @brief The accept key. */
    AcceptKey = 0x1E,
    /** @brief The mode change key. */
    ModeChangeKey = 0x1F,

    /** @brief The spacebar key. */
    SpaceKey = 0x20,
    /** @brief The prior page/page up key. */
    PriorKey = 0x21,
    /** @brief The next page/page down key. */
    NextKey = 0x22,
    /** @brief The end key. */
    EndKey = 0x23,
    /** @brief The home key. */
    HomeKey = 0x24,
    /** @brief The left arrow key. */
    LeftKey = 0x25,
    /** @brief The up arrow key. */
    UpKey = 0x26,
    /** @brief The right arrow key. */
    RightKey = 0x27,
    /** @brief The down arrow key. */
    DownKey = 0x28,
    /** @brief The select key. */
    SelectKey = 0x29,
    /** @brief The print key. */
    PrintKey = 0x2A,
    /** @brief The execute key. */
    ExecuteKey = 0x2B,
    /** @brief The snapshot key. */
    SnapshotKey = 0x2C,
    /** @brief The insert key. */
    InsertKey = 0x2D,
    /** @brief The delete key. */
    DeleteKey = 0x2E,
    /** @brief The help key. */
    HelpKey = 0x2F,

    // --- MAIN NUMBER KEYS (0-9 Above Letters) ---
    /** @brief The number 0 key. */
    Num0Key = 0x30,
    /** @brief The number 1 key. */
    Num1Key = 0x31,
    /** @brief The number 2 key. */
    Num2Key = 0x32,
    /** @brief The number 3 key. */
    Num3Key = 0x33,
    /** @brief The number 4 key. */
    Num4Key = 0x34,
    /** @brief The number 5 key. */
    Num5Key = 0x35,
    /** @brief The number 6 key. */
    Num6Key = 0x36,
    /** @brief The number 7 key. */
    Num7Key = 0x37,
    /** @brief The number 8 key. */
    Num8Key = 0x38,
    /** @brief The number 9 key. */
    Num9Key = 0x39,

    /** @brief The A key. */
    AKey = 0x41,
    /** @brief The B key. */
    BKey = 0x42,
    /** @brief The C key. */
    CKey = 0x43,
    /** @brief The D key. */
    DKey = 0x44,
    /** @brief The E key. */
    EKey = 0x45,
    /** @brief The F key. */
    FKey = 0x46,
    /** @brief The G key. */
    GKey = 0x47,
    /** @brief The H key. */
    HKey = 0x48,
    /** @brief The I key. */
    IKey = 0x49,
    /** @brief The J key. */
    JKey = 0x4A,
    /** @brief The K key. */
    KKey = 0x4B,
    /** @brief The L key. */
    LKey = 0x4C,
    /** @brief The M key. */
    MKey = 0x4D,
    /** @brief The N key. */
    NKey = 0x4E,
    /** @brief The O key. */
    OKey = 0x4F,
    /** @brief The P key. */
    PKey = 0x50,
    /** @brief The Q key. */
    QKey = 0x51,
    /** @brief The R key. */
    RKey = 0x52,
    /** @brief The S key. */
    SKey = 0x53,
    /** @brief The T key. */
    TKey = 0x54,
    /** @brief The U key. */
    UKey = 0x55,
    /** @brief The V key. */
    VKey = 0x56,
    /** @brief The W key. */
    WKey = 0x57,
    /** @brief The X key. */
    XKey = 0x58,
    /** @brief The Y key. */
    YKey = 0x59,
    /** @brief The Z key. */
    ZKey = 0x5A,

    /** @brief The left Windows/Super key. */
    LWinKey = 0x5B,
    /** @brief The right Windows/Super key. */
    RWinKey = 0x5C,
    /** @brief The applications key. */
    AppsKey = 0x5D,

    /** @brief The sleep key. */
    SleepKey = 0x5F,

    // --- NUMERIC KEYPAD ---
    /** @brief The numpad 0 key. */
    Numpad0Key = 0x60,
    /** @brief The numpad 1 key. */
    Numpad1Key = 0x61,
    /** @brief The numpad 2 key. */
    Numpad2Key = 0x62,
    /** @brief The numpad 3 key. */
    Numpad3Key = 0x63,
    /** @brief The numpad 4 key. */
    Numpad4Key = 0x64,
    /** @brief The numpad 5 key. */
    Numpad5Key = 0x65,
    /** @brief The numpad 6 key. */
    Numpad6Key = 0x66,
    /** @brief The numpad 7 key. */
    Numpad7Key = 0x67,
    /** @brief The numpad 8 key. */
    Numpad8Key = 0x68,
    /** @brief The numpad 9 key. */
    Numpad9Key = 0x69,
    /** @brief The numpad multiply key. */
    MultiplyKey = 0x6A,
    /** @brief The numpad add key. */
    AddKey = 0x6B,
    /** @brief The numpad separator key. */
    SeparatorKey = 0x6C,
    /** @brief The numpad subtract key. */
    SubtractKey = 0x6D,
    /** @brief The numpad decimal key. */
    DecimalKey = 0x6E,
    /** @brief The numpad divide key. */
    DivideKey = 0x6F,
    
    /** @brief The F1 function key. */
    F1Key = 0x70,
    /** @brief The F2 function key. */
    F2Key = 0x71,
    /** @brief The F3 function key. */
    F3Key = 0x72,
    /** @brief The F4 function key. */
    F4Key = 0x73,
    /** @brief The F5 function key. */
    F5Key = 0x74,
    /** @brief The F6 function key. */
    F6Key = 0x75,
    /** @brief The F7 function key. */
    F7Key = 0x76,
    /** @brief The F8 function key. */
    F8Key = 0x77,
    /** @brief The F9 function key. */
    F9Key = 0x78,
    /** @brief The F10 function key. */
    F10Key = 0x79,
    /** @brief The F11 function key. */
    F11Key = 0x7A,
    /** @brief The F12 function key. */
    F12Key = 0x7B,
    /** @brief The F13 function key. */
    F13Key = 0x7C,
    /** @brief The F14 function key. */
    F14Key = 0x7D,
    /** @brief The F15 function key. */
    F15Key = 0x7E,
    /** @brief The F16 function key. */
    F16Key = 0x7F,
    /** @brief The F17 function key. */
    F17Key = 0x80,
    /** @brief The F18 function key. */
    F18Key = 0x81,
    /** @brief The F19 function key. */
    F19Key = 0x82,
    /** @brief The F20 function key. */
    F20Key = 0x83,
    /** @brief The F21 function key. */
    F21Key = 0x84,
    /** @brief The F22 function key. */
    F22Key = 0x85,
    /** @brief The F23 function key. */
    F23Key = 0x86,
    /** @brief The F24 function key. */
    F24Key = 0x87,

    /** @brief The num lock key. */
    NumLockKey = 0x90,
    /** @brief The scroll lock key. */
    ScrollKey = 0x91,
    /** @brief The numpad equal key. */
    NumpadEqualKey = 0x92,

    /** @brief The left shift key. */
    LShiftKey = 0xA0,
    /** @brief The right shift key. */
    RShiftKey = 0xA1,
    /** @brief The left control key. */
    LControlKey = 0xA2,
    /** @brief The right control key. */
    RControlKey = 0xA3,
    /** @brief The left alt key. */
    LAltKey = 0xA4,
    /** @brief The right alt key. */
    RAltKey = 0xA5,

    // --- PUNCTUATION AND SYMBOLS ---
    /** @brief The semicolon key (; / :). */
    SemicolonKey = 0xBA,
    /** @brief The plus key (= / +). */
    PlusKey = 0xBB,
    /** @brief The comma key (, / <). */
    CommaKey = 0xBC,
    /** @brief The minus key (- / _). */
    MinusKey = 0xBD,
    /** @brief The period key (. / >). */
    PeriodKey = 0xBE,
    /** @brief The slash key (/ / ?). */
    SlashKey = 0xBF,
    /** @brief The grave accent key (~). */
    GraveKey = 0xC0,
    
    /** @brief The left bracket key ([ / {). */
    LBracketKey = 0xDB,
    /** @brief The backslash key (\ / |). */
    BackslashKey = 0xDC,
    /** @brief The right bracket key (] / }). */
    RBracketKey = 0xDD,
    /** @brief The quote key (' / "). */
    QuoteKey = 0xDE,

    /** @brief Total count of available keys. */
    KeysCount
} Keys;

/**
 * @brief Initializes the input system. Call twice; once to obtain memory requirement (passing
 * state = 0), then a second time passing allocated memory to state.
 * 
 * @param MemoryRequirement The required size of the state memory.
 * @param State Either 0 or the allocated block of state memory.
 */
void InputSystemInitialize(UInt64* MemoryRequirement, void* State);

/**
 * @brief Shuts the input system down.
 * @param State A pointer to the system state.
 */
void InputSystemShutdown(void* State);

/**
 * @brief Updates the input system every frame.
 * @param DeltaTime The delta time in seconds since the last frame.
 */
void InputUpdate(Float64 DeltaTime);

// --- KEYBOARD INPUT ---

/**
 * @brief Indicates if the given key is currently pressed down.
 * @param Key They key to be checked.
 * @returns True if currently pressed; otherwise false.
 */
FAPI Bool8 InputIsKeyDown(Keys Key);

/**
 * @brief Indicates if the given key is NOT currently pressed down.
 * @param Key They key to be checked.
 * @returns True if currently released; otherwise false.
 */
FAPI Bool8 InputIsKeyUp(Keys Key);

/**
 * @brief Indicates if the given key was previously pressed down on the last frame.
 * @param Key They key to be checked.
 * @returns True if was previously pressed; otherwise false.
 */
FAPI Bool8 InputWasKeyDown(Keys Key);

/**
 * @brief Indicates if the given key was previously pressed down in the last frame.
 * @param Key They key to be checked.
 * @returns True if previously released; otherwise false.
 */
FAPI Bool8 InputWasKeyUp(Keys Key);

/**
 * @brief Sets the state for the given key.
 * @param Key The key to be processed.
 * @param Pressed Indicates whether the key is currently pressed.
 */
void InputProcessKey(Keys Key, Bool8 Pressed);

// --- MOUSE INPUT ---

/**
 * @brief Indicates if the given mouse button is currently pressed.
 * @param Button The button to check.
 * @returns True if currently pressed; otherwise false.
 */
FAPI Bool8 InputIsMouseButtonDown(MouseButtons Button);

/**
 * @brief Indicates if the given mouse button is currently released.
 * @param Button The button to check.
 * @returns True if currently released; otherwise false.
 */
FAPI Bool8 InputIsMouseButtonUp(MouseButtons Button);

/**
 * @brief Indicates if the given mouse button was previously pressed in the last frame.
 * @param Button The button to check.
 * @returns True if previously pressed; otherwise false.
 */
FAPI Bool8 InputWasMouseButtonDown(MouseButtons Button);
/**
 * @brief Indicates if the given mouse button was previously released in the last frame.
 * @param Button The button to check.
 * @returns True if previously released; otherwise false.
 */
FAPI Bool8 InputWasMouseButtonUp(MouseButtons Button);

/** 
 * @brief Obtains the current mouse position.
 * @param X A pointer to hold the current mouse position on the x-axis.
 * @param Y A pointer to hold the current mouse position on the y-axis.
 */
FAPI void InputGetMousePosition(Int32 * X, Int32 * Y);

/** 
 * @brief Obtains the previous mouse position.
 * @param X A pointer to hold the previous mouse position on the x-axis.
 * @param Y A pointer to hold the previous mouse position on the y-axis.
 */
FAPI void InputGetPreviousMousePosition(Int32* X, Int32* Y);

/**
 * @brief Sets the press state of the given mouse button.
 * @param Button The mouse button whose state to set.
 * @param Pressed Indicates if the mouse button is currently pressed.
 */
void InputProcessMouseButton(MouseButtons Button, Bool8 Pressed);
/**
 * @brief Sets the current position of the mouse to the given X and Y positions.
 * Also updates the previous position beforehand.
 */
void InputProcessMouseMove(Int16 X, Int16 Y);

/**
 * @brief Processes mouse wheel scrolling.
 * @param WheelDelta The amount of scrolling which occurred on the z axis (mouse wheel)
 */
void InputProcessMouseWheel(Int8 WheelDelta);