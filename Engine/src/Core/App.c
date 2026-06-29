#include "App.h"
#include "GameInstance.h"

#include "Logger.h"

#include "Platform/Platform.h"
#include "Memory.h"
#include "Event.h"
#include "Input.h"
#include "Clock.h"

#include "Renderer/RendererFrontend.h"

// Application configuration.
typedef struct AppState {
    Game* Instance;
    Bool8 IsRunning;
    Bool8 IsSuspended;
    PlatformState Platform;
    Int16 Width;
    Int16 Height;
    Clock Clock;
    Float64 PreviousTime;
} AppState;

static Bool8 Initialized;
static AppState State;

Bool8 AppOnEvent(UInt16 Code, void* Sender, void* Reciever, EventContext Context);

Bool8 AppOnKeyEvent(UInt16 Code, void* Sender, void* Reciever, EventContext Context);
Bool8 AppOnMouseButtonEvent(UInt16 Code, void* Sender, void* Reciever, EventContext Context);

Bool8 AppCreate(Game* Instance)
{
    if (Initialized)
    {
        FLFATAL("AppCreate() function was called more than once");
        return FALSE;
    }

    State.Instance = Instance;

    //Initializes Subsystems
    LogCreate();
    InputSystemInitialize();

    //TODO: Remove This
    FLFATAL("Crash Code %d: Testing Fatal Logs, not Actually fatal", 67);
    FLERROR("Error Code %d: This is not an error, just another test", 55);
    FLWARN("This in not warning of any danger, just if a flaw is found, but does not ruin the ENTIRE program. Test Var: ->%f", 3.14159);
    FLINFO("Printing messages is fun somewhat Test Var: ->%f", 3.14159);
    FLDEBUG("Just some Debugging like over here  ->  Test Var: ->%f", 3.14159);
    FLTRACE("Tracing a line somewhere... Over Here!   ->    Test Var: ->%f", 3.14159);

    State.IsRunning = TRUE;
    State.IsSuspended = FALSE;

    if (!EventSystemInitialize())
    {
        FLERROR("Event System failed to initialize, Application can't run");
        return FALSE;
    }

    EventRegister(EVENT_APP_QUIT, 0, AppOnEvent);

    EventRegister(EVENT_KEY_DOWN, 0, AppOnKeyEvent);
    EventRegister(EVENT_KEY_UP, 0, AppOnKeyEvent);

    EventRegister(EVENT_MOUSE_DOWN, 0, AppOnMouseButtonEvent);
    EventRegister(EVENT_MOUSE_UP, 0, AppOnMouseButtonEvent);

    if (!PlatformStartup(
        &State.Platform, 
        Instance->Config.Name, 
        Instance->Config.StartX, 
        Instance->Config.StartY, 
        Instance->Config.StartWidth, 
        Instance->Config.StartHeight))
    {
        return FALSE;
    }

    if (!RendererInitialize(Instance->Config.Name, &State.Platform))
    {
        FLFATAL("Failed to Initialize Renderer! Aborting Application");
    }

    if (!State.Instance->Initialize(State.Instance))
    {
        FLFATAL("Game Failed to Initalize");
        return FALSE;
    }

    State.Instance->OnResize(State.Instance, State.Width, State.Height);
    
    Initialized = true;

    return TRUE;
}

Bool8 AppRun()
{
    ClockStart(&State.Clock);
    ClockUpdate(&State.Clock);
    State.PreviousTime = State.Clock.Elapsed;
    Float64 RunningTime = 0;
    UInt8 FrameCount = 0;
    Float64 TargetFrameRate = 120.0;
    Float64 TargetFrameSeconds = 1.0 / TargetFrameRate;

    FLDEBUG("%s", FMGetMemoryUsageString());

    while(State.IsRunning)
        {
            if (!PlatformPollEvents(&State.Platform))
            {
                State.IsRunning = FALSE;
            }

            if (!State.IsSuspended)
            {
                ClockUpdate(&State.Clock);
                Float64 CurrentTime = State.Clock.Elapsed;
                Float64 DeltaTime = (CurrentTime - State.PreviousTime);
                Float64 FrameStartTime = PlatformGetAbsoluteTime();

                if (!State.Instance->Update(State.Instance, (Float32)DeltaTime))
                {
                    FLFATAL("The Game's Update Function failed! shutting down application");
                    State.IsRunning = FALSE;
                    break;
                }

                // Calls Render Routine
                if (!State.Instance->Render(State.Instance, (Float32)DeltaTime))
                {
                    FLFATAL("The Game's Render Function failed! shutting down application");
                    State.IsRunning = FALSE;
                    break;
                }

                // TODO: Refactor Packet Creation
                RenderPacket Packet;
                Packet.DeltaTime = (Float32)DeltaTime;
                RendererDrawFrame(&Packet);

                Float64 FrameEndTime = PlatformGetAbsoluteTime();
                Float64 FrameElapsedTime = FrameEndTime - FrameStartTime;
                RunningTime += FrameElapsedTime;
                Float64 RemainingSeconds = TargetFrameSeconds - FrameElapsedTime;

                if (RemainingSeconds > 0)
                {
                    UInt64 RemainingMiliseconds = (UInt64)(RemainingSeconds * 1000);

                    Bool8 LimitFrames = FALSE;
                    if (RemainingMiliseconds > 0 && LimitFrames)
                    {
                        PlatformSleep(RemainingMiliseconds - 1);
                    }
                }
                FrameCount++;

            // NOTE: Input update/state copying should always be handled
            // after any input should be recorded; I.E. before this line.
            // As a safety, input is the last thing to be updated before
            // this frame ends.
            InputUpdate(DeltaTime);

                State.PreviousTime = CurrentTime;
            }
        }

    State.IsRunning = FALSE;

    EventUnregister(EVENT_APP_QUIT, 0, AppOnEvent);

    EventUnregister(EVENT_KEY_DOWN, 0, AppOnKeyEvent);
    EventUnregister(EVENT_KEY_UP, 0, AppOnKeyEvent);

    EventUnregister(EVENT_MOUSE_DOWN, 0, AppOnMouseButtonEvent);
    EventUnregister(EVENT_MOUSE_UP, 0, AppOnMouseButtonEvent);

    EventSystemShutdown();
    InputSystemShutdown();

    RendererShutdown();

    PlatformShutdown(&State.Platform);

    return TRUE;
}

Bool8 AppOnEvent(UInt16 Code, void* Sender, void* Reciever, EventContext Context)
{
    (void)Reciever;
    (void)Sender;
    (void)Context;

    switch (Code)
    {
        case EVENT_APP_QUIT:
        {
            FLDEBUG("Event Code APP_QUIT Recieved, Shutting Down.");
            State.IsRunning = FALSE;
            return TRUE;
        }
    }
    return FALSE;
}

Bool8 AppOnKeyEvent(UInt16 Code, void* Sender, void* Reciever, EventContext Context)
{
    (void)Reciever;
    (void)Sender;
    if (Code == EVENT_KEY_DOWN) {
        UInt16 KeyCode = Context.Data.UInt16[0];
        if (KeyCode == EscapeKey) {
            // NOTE: Technically firing an event to itself, but there may be other listeners.
            EventContext Data = {0};
            EventFire(EVENT_APP_QUIT, 0, Data);

            // Block anything else from processing this.
            return TRUE;
        } else if (KeyCode == AKey) {
            // Example on checking for a key
            FLDEBUG("Explicit - A key pressed!");
        } else {
            FLTRACE("'%c' key pressed in window.", KeyCode);
        }
    } else if (Code == EVENT_KEY_UP) {
        UInt16 KeyCode = Context.Data.UInt16[0];
        if (KeyCode == BKey) {
            // Example on checking for a key
            FLDEBUG("Explicit - B key released!");
        } else {
            FLTRACE("'%c' key released in window.", KeyCode);
        }
    }
    return FALSE;
}

Bool8 AppOnMouseButtonEvent(UInt16 Code, void* Sender, void* Reciever, EventContext Context)
{
    (void)Reciever;
    (void)Sender;
    if (Code == EVENT_MOUSE_DOWN)
    {
        UInt16 ButtonCode = Context.Data.UInt16[0];
        
        if (ButtonCode == MouseButtonRight)
        {
            FLDEBUG("Explicit - Right mouse button pressed!");
        } else {
            FLTRACE("Mouse button %d pressed in window.", ButtonCode);
        }
        
    } else if (Code == EVENT_MOUSE_UP)
    {
        UInt16 ButtonCode = Context.Data.UInt16[0];
        
        if (ButtonCode == MouseButtonLeft) {
            FLDEBUG("Explicit - Left mouse button released!");
        } else {
            FLTRACE("Mouse button %d released in window.", ButtonCode);
        }
    }
    return FALSE;
}