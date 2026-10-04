#include "Core/Input.h"
#include "Core/Event.h"
#include "Core/Memory.h"
#include "Core/Logger.h"

typedef struct KeyboardState {
    Bool8 Keys[256];
} KeyboardState;

typedef struct MouseState {
    Int16 X;
    Int16 Y;
    UInt8 MouseButtons[ButtonCount];
} MouseState;

typedef struct InputState {
    KeyboardState KeyboardCurrent;
    KeyboardState KeyboardPrevious;
    MouseState MouseCurrent;
    MouseState MousePrevious;
} InputState;

static InputState* StatePtr;

void InputSystemInitialize(UInt64* MemoryRequirement, void* State) 
{
    *MemoryRequirement = (UInt64)sizeof(InputState);
    if (State == 0) 
    {
        return;
    }

    FMZeroMemory(State, sizeof(InputState));
    StatePtr = State;

    (void)MemoryRequirement;

    FLINFO("Input subsystem initialized.");
}

void InputSystemShutdown(void* State)
{
    //TODO: Shutdown Routines when needed
    (void)State;
    StatePtr = 0;
}

void InputUpdate(Float64 DeltaTime)
{
    (void)DeltaTime;
    if (!StatePtr)
    {
        return;
    }

    // Copies current state into Previous
    FMCopyMemory(&StatePtr->KeyboardPrevious, &StatePtr->KeyboardCurrent, sizeof(KeyboardState));
    FMCopyMemory(&StatePtr->MousePrevious, &StatePtr->MouseCurrent, sizeof(MouseState));
}

void InputProcessKey(Keys Key, Bool8 Pressed)
{
    // Only handle this if the state actually changed.
    if (StatePtr->KeyboardCurrent.Keys[Key] != Pressed)
    {
        // Update internal state.
        StatePtr->KeyboardCurrent.Keys[Key] = Pressed;

        // Fire off an event for immediate processing.
        EventContext Context;
        Context.Data.UInt16[0] = Key;
        EventFire(Pressed ? EVENT_KEY_DOWN : EVENT_KEY_UP, 0, Context);
    }
}

void InputProcessMouseButton(MouseButtons Button, Bool8 Pressed)
{
    if (StatePtr->MouseCurrent.MouseButtons[Button] != Pressed)
    {
        StatePtr->MouseCurrent.MouseButtons[Button] = Pressed;

        EventContext Context;
        Context.Data.UInt16[0] = Button;
        EventFire(Pressed ? EVENT_MOUSE_DOWN : EVENT_MOUSE_UP, 0, Context);
    }
}

void InputProcessMouseMove(Int16 X, Int16 Y)
{
    if (StatePtr->MouseCurrent.X != X || StatePtr->MouseCurrent.Y != Y)
    {
        //NOTE: Enable this if debugging mouse input.
        //FLTRACE("Mouse Position: (%i, %i)", X, Y);

        // Update internal state.
        StatePtr->MouseCurrent.X = X;
        StatePtr->MouseCurrent.Y = Y;

        // Fire the event.
        EventContext Context;
        Context.Data.UInt16[0] = X;
        Context.Data.UInt16[1] = Y;
        EventFire(EVENT_MOUSE_MOVE, 0, Context);
    }
}

void InputProcessMouseWheel(Int8 WheelDelta)
{
    EventContext Context;
    Context.Data.UInt16[0] = WheelDelta;
    EventFire(EVENT_MOUSE_SCROLL, 0, Context);
}

// --- KEYBOARD INPUT ---
Bool8 InputIsKeyDown(Keys Key)
{
    if (!StatePtr)
    {
        return false;
    }
    return StatePtr->KeyboardCurrent.Keys[Key] == true;
}
Bool8 InputIsKeyUp(Keys Key)
{
    if (!StatePtr)
    {
        return true;
    }
    return StatePtr->KeyboardCurrent.Keys[Key] == false;
}
Bool8 InputWasKeyDown(Keys Key)
{
    if (!StatePtr)
    {
        return false;
    }
    return StatePtr->KeyboardPrevious.Keys[Key] == true;
}
Bool8 InputWasKeyUp(Keys Key)
{
    if (!StatePtr)
    {
        return true;
    }
    return StatePtr->KeyboardPrevious.Keys[Key] == false;
}

// --- MOUSE INPUT ---
Bool8 InputIsMouseButtonDown(MouseButtons Button)
{
    if (!StatePtr)
    {
        return false;
    }
    return StatePtr->MouseCurrent.MouseButtons[Button] == true;
}
Bool8 InputIsMouseButtonUp(MouseButtons Button)
{
    if (!StatePtr)
    {
        return true;
    }
    return StatePtr->MouseCurrent.MouseButtons[Button] == false;
}
Bool8 InputWasMouseButtonDown(MouseButtons Button)
{
    if (!StatePtr)
    {
        return false;
    }
    return StatePtr->MousePrevious.MouseButtons[Button] == true;
}
Bool8 InputWasMouseButtonUp(MouseButtons Button)
{
    if (!StatePtr)
    {
        return true;
    }
    return StatePtr->MousePrevious.MouseButtons[Button] == false;
}

// --- MOUSE MOVEMENT ---
void InputGetMousePosition(Int32 * X, Int32 * Y)
{
    if (!StatePtr)
    {
        *X = 0;
        *Y = 0;
        return;
    }
    *X = StatePtr->MouseCurrent.X;
    *Y = StatePtr->MouseCurrent.Y;
}
void InputGetPreviousMousePosition(Int32* X, Int32* Y)
{
 if (!StatePtr)
    {
        *X = 0;
        *Y = 0;
        return;
    }
    *X = StatePtr->MousePrevious.X;
    *Y = StatePtr->MousePrevious.Y;
}