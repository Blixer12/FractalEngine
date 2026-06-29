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

static Bool8 Initialized;
static InputState State;

void InputSystemInitialize()
{
    FMZeroMemory(&State, sizeof(InputState));
    Initialized = TRUE;
    FLINFO("Input Subsystem Intialized");
}

void InputSystemShutdown()
{
    //TODO: Shutdown Routines when needed
    Initialized = FALSE;
}

void InputUpdate(Float64 DeltaTime)
{
    (void)DeltaTime;
    if (!Initialized)
    {
        return;
    }

    // Copies current state into Previous
    FMCopyMemory(&State.KeyboardPrevious, &State.KeyboardCurrent, sizeof(KeyboardState));
    FMCopyMemory(&State.MousePrevious, &State.MouseCurrent, sizeof(MouseState));
}

void InputProccessKey(Keys Key, Bool8 Pressed)
{
    // Only handle this if the state actually changed.
    if (State.KeyboardCurrent.Keys[Key] != Pressed)
    {
        // Update internal state.
        State.KeyboardCurrent.Keys[Key] = Pressed;

        // Fire off an event for immediate processing.
        EventContext Context;
        Context.Data.UInt16[0] = Key;
        EventFire(Pressed ? EVENT_KEY_DOWN : EVENT_KEY_UP, 0, Context);
    }
}

void InputProcessMouseButton(MouseButtons Button, Bool8 Pressed)
{
    if (State.MouseCurrent.MouseButtons[Button] != Pressed)
    {
        State.MouseCurrent.MouseButtons[Button] = Pressed;

        EventContext Context;
        Context.Data.UInt16[0] = Button;
        EventFire(Pressed ? EVENT_MOUSE_DOWN : EVENT_MOUSE_UP, 0, Context);
    }
}

void InputProcessMouseMove(Int16 X, Int16 Y)
{
    if (State.MouseCurrent.X != X || State.MouseCurrent.Y != Y)
    {
        //NOTE: Enable this if debugging mouse input.
        //FLTRACE("Mouse Position: (%i, %i)", X, Y);

        // Update internal state.
        State.MouseCurrent.X = X;
        State.MouseCurrent.Y = Y;

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
    if (!Initialized)
    {
        return FALSE;
    }
    return State.KeyboardCurrent.Keys[Key] == TRUE;
}
Bool8 InputIsKeyUp(Keys Key)
{
    if (!Initialized)
    {
        return TRUE;
    }
    return State.KeyboardCurrent.Keys[Key] == FALSE;
}
Bool8 InputWasKeyDown(Keys Key)
{
    if (!Initialized)
    {
        return FALSE;
    }
    return State.KeyboardPrevious.Keys[Key] == TRUE;
}
Bool8 InputWasKeyUp(Keys Key)
{
    if (!Initialized)
    {
        return TRUE;
    }
    return State.KeyboardPrevious.Keys[Key] == FALSE;
}

// --- MOUSE INPUT ---
Bool8 InputIsMouseButtonDown(MouseButtons Button)
{
    if (!Initialized)
    {
        return FALSE;
    }
    return State.MouseCurrent.MouseButtons[Button] == TRUE;
}
Bool8 InputIsMouseButtonUp(MouseButtons Button)
{
    if (!Initialized)
    {
        return TRUE;
    }
    return State.MouseCurrent.MouseButtons[Button] == FALSE;
}
Bool8 InputWasMouseButtonDown(MouseButtons Button)
{
    if (!Initialized)
    {
        return FALSE;
    }
    return State.MousePrevious.MouseButtons[Button] == TRUE;
}
Bool8 InputWasMouseButtonUp(MouseButtons Button)
{
    if (!Initialized)
    {
        return TRUE;
    }
    return State.MousePrevious.MouseButtons[Button] == FALSE;
}

// --- MOUSE MOVEMENT ---
void InputGetMousePosition(Int32 * X, Int32 * Y)
{
    if (!Initialized)
    {
        *X = 0;
        *Y = 0;
        return;
    }
    *X = State.MouseCurrent.X;
    *Y = State.MouseCurrent.Y;
}
void InputGetPreviousMousePosition(Int32* X, Int32* Y)
{
 if (!Initialized)
    {
        *X = 0;
        *Y = 0;
        return;
    }
    *X = State.MousePrevious.X;
    *Y = State.MousePrevious.Y;
}