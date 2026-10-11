#include "App.h"
#include "GameInstance.h"

#include "Logger.h"

#include "Platform/Platform.h"

#include "Core/Memory.h"
#include "Core/Event.h"
#include "Core/Input.h"
#include "Core/Clock.h"
#include "Core/FString.h"

#include "Memory/LinearAllocator.h"

#include "Renderer/CrystalFrontend.h"

// Systems
#include "Systems/TextureSystem.h"
#include "Systems/MaterialSystem.h"
#include "Systems/GeometrySystem.h"
#include "Systems/ResourceSystem.h"

#include "Math/FMath.h"

// Application configuration.
typedef struct AppState {
    Game* Instance;
    Bool8 IsRunning;
    Bool8 IsSuspended;
    Int16 Width;
    Int16 Height;
    Clock Clock;
    Float64 PreviousTime;
    LinearAllocator SystemAllocator;

    // Core
    UInt64 EventSystemMemoryRequirement;
    void* EventState;

    UInt64 MemorySystemMemoryRequirement;
    void* MemoryState;

    UInt64 LogSystemMemoryRequirement;
    void* LogState;

    UInt64 InputSystemMemoryRequirement;
    void* InputState;

    UInt64 PlatformSystemMemoryRequirement;
    void* PlatformState;

    UInt64 ResourceSystemMemoryRequirement;
    void* ResourceState;

    // Renderer
    UInt64 CrystalSystemMemoryRequirement;
    void* CrystalState;

    // Systems
    UInt64 TextureSystemMemoryRequirement;
    void* TextureState;

    UInt64 MaterialSystemMemoryRequirement;
    void* MaterialState;

    UInt64 GeometrySystemMemoryRequirement;
    void* GeometryState;

    Geometry* TestGeometry;
} AppState;

static AppState* State;

void AppGetWindowSize(UInt32* Width, UInt32* Height) 
{
    if (Width && Height) {
        *Width = State->Width;
        *Height = State->Height;
    }
}

Bool8 AppOnEvent(UInt16 Code, void* Sender, void* Reciever, EventContext Context);

Bool8 AppOnKeyEvent(UInt16 Code, void* Sender, void* Reciever, EventContext Context);
Bool8 AppOnMouseButtonEvent(UInt16 Code, void* Sender, void* Reciever, EventContext Context);

Bool8 AppOnWindowResize(UInt16 Code, void* Sender, void* Reciever, EventContext Context);

Bool8 EventOnDebugEvent(UInt16 Code, void* Sender, void* Reciever, EventContext Data)
{
    (void)Code;
    (void)Sender;
    (void)Reciever;
    (void)Data;

    const char* Names[4] = {
        "Cobblestone",
        "Paving",
        "Paving2",
        "WhiteStone"};
    static Int8 Choice = 3;

    const char* OldName = Names[Choice];

    Choice++;
    Choice %= 4;

    // Acquire the new texture
    State->TestGeometry->Material->BaseColorMap.Texture = TextureSystemAcquire(Names[Choice], true);
    if (!State->TestGeometry->Material->BaseColorMap.Texture)
    {
        FLWARN("EventOnDebugEvent - No Texture! using default!");
        State->TestGeometry->Material->BaseColorMap.Texture = TextureSystemGetDefault();
    }

    TextureSystemRelease(OldName);
    return true;
}

Bool8 AppCreate(Game* Instance)
{
    if (Instance->AppState)
    {
        FLFATAL("AppCreate() function was called more than once");
        return false;
    }

    Instance->AppState = FMAllocate(sizeof(AppState), MEMORY_TAG_APPLICATION);
    State = Instance->AppState;
    State->Instance = Instance;
    State->IsRunning = false;
    State->IsSuspended = false;

    UInt64 SystemAllocatorTotalSize = 64 * 1024 * 1024; // 64MB
    LinearAllocatorCreate(SystemAllocatorTotalSize, 0, &State->SystemAllocator);
        
    

    // Initializes Subsystems

    // Events
    EventSystemInitialize(&State->EventSystemMemoryRequirement, 0);
    State->EventState = LinearAllocatorAllocate(&State->SystemAllocator, State->EventSystemMemoryRequirement);
    EventSystemInitialize(&State->EventSystemMemoryRequirement, State->EventState);

    // Memory
    MemorySystemInitialize(&State->MemorySystemMemoryRequirement, 0);
    State->MemoryState = LinearAllocatorAllocate(&State->SystemAllocator, State->MemorySystemMemoryRequirement);
    MemorySystemInitialize(&State->MemorySystemMemoryRequirement, State->MemoryState);

    // Logging
    LogCreate(&State->LogSystemMemoryRequirement, 0);
    State->LogState = LinearAllocatorAllocate(&State->SystemAllocator, State->LogSystemMemoryRequirement);

    if(!LogCreate(&State->LogSystemMemoryRequirement, State->LogState))
    {
        FLERROR("Failed to Create Log system: Shutting Down...");
        return false;
    }

    InputSystemInitialize(&State->InputSystemMemoryRequirement, 0);
    State->InputState = LinearAllocatorAllocate(&State->SystemAllocator, State->InputSystemMemoryRequirement);
    InputSystemInitialize(&State->InputSystemMemoryRequirement, State->InputState);

    EventRegister(EVENT_APP_QUIT, 0, AppOnEvent);

    EventRegister(EVENT_KEY_DOWN, 0, AppOnKeyEvent);
    EventRegister(EVENT_KEY_UP, 0, AppOnKeyEvent);

    EventRegister(EVENT_MOUSE_DOWN, 0, AppOnMouseButtonEvent);
    EventRegister(EVENT_MOUSE_UP, 0, AppOnMouseButtonEvent);

    EventRegister(EVENT_RESIZED, 0, AppOnWindowResize);

    EventRegister(EVENT_DEBUG0, 0, EventOnDebugEvent);

    PlatformSystemStartup(&State->PlatformSystemMemoryRequirement, 0, 0, 0, 0, 0, 0);
    State->PlatformState = LinearAllocatorAllocate(&State->SystemAllocator, State->PlatformSystemMemoryRequirement);
    if (!PlatformSystemStartup(
        &State->PlatformSystemMemoryRequirement,
        State->PlatformState, 
        Instance->Config.Name, 
        Instance->Config.StartX, 
        Instance->Config.StartY, 
        Instance->Config.StartWidth, 
        Instance->Config.StartHeight))
    {
        return false;
    }

    // Resource System
    ResourceSystemConfig ResourceSysConfig;
    ResourceSysConfig.AssetBasePath = "../Assets";
    ResourceSysConfig.MaxLoaderCount = 32;

    ResourceSystemInitialize(&State->ResourceSystemMemoryRequirement, 0, ResourceSysConfig);
    State->ResourceState = LinearAllocatorAllocate(&State->SystemAllocator, State->ResourceSystemMemoryRequirement);

    if (!ResourceSystemInitialize(&State->ResourceSystemMemoryRequirement, State->ResourceState, ResourceSysConfig)) {
        FLFATAL("Failed to initialize Resource system. Application cannot continue.");
        return false;
    }

    CrystalInitialize(&State->CrystalSystemMemoryRequirement, 0, 0);
    State->CrystalState = LinearAllocatorAllocate(&State->SystemAllocator, State->CrystalSystemMemoryRequirement);

    if (!CrystalInitialize(&State->CrystalSystemMemoryRequirement, State->CrystalState, Instance->Config.Name))
    {
        FLFATAL("Failed to Initialize Renderer! Aborting Application");
    }

    // Texture system.
    TextureSystemConfig TextureSysConfig;
    TextureSysConfig.MaxTextureCount = 65536;

    TextureSystemInitialize(&State->TextureSystemMemoryRequirement, 0, TextureSysConfig);
    State->TextureState = LinearAllocatorAllocate(&State->SystemAllocator, State->TextureSystemMemoryRequirement);

    if (!TextureSystemInitialize(&State->TextureSystemMemoryRequirement, State->TextureState, TextureSysConfig)) {
        FLFATAL("Failed to initialize texture system. Application cannot continue.");
        return false;
    }

    // Material system
    MaterialSystemConfig MaterialSysConfig;
    MaterialSysConfig.MaxMaterialCount = 4096;

    MaterialSystemInitialize(&State->MaterialSystemMemoryRequirement, 0, MaterialSysConfig);
    State->MaterialState = LinearAllocatorAllocate(&State->SystemAllocator, State->MaterialSystemMemoryRequirement);

    if (!MaterialSystemInitialize(&State->MaterialSystemMemoryRequirement, State->MaterialState, MaterialSysConfig)) {
        FLFATAL("Failed to initialize material system. Application cannot continue.");
        return false;
    }

    // Geometry system
    GeometrySystemConfig GeometrySysConfig;
    GeometrySysConfig.MaxGeometryCount = 4096;

    GeometrySystemInitialize(&State->GeometrySystemMemoryRequirement, 0, GeometrySysConfig);
    State->GeometryState = LinearAllocatorAllocate(&State->SystemAllocator, State->GeometrySystemMemoryRequirement);

    if (!GeometrySystemInitialize(&State->GeometrySystemMemoryRequirement, State->GeometryState, GeometrySysConfig)) {
        FLFATAL("Failed to initialize geometry system. Application cannot continue.");
        return false;
    }

    GeometryConfig GConfig = GeometrySystemGeneratePlaneConfig(10.0f, 10.0f, 5, 5, 2.0f, 2.0f, "Test Geometry", "TestMaterial");
    State->TestGeometry = GeometrySystemAcquireFromConfig(GConfig, true);

    FMFree(GConfig.Vertices, sizeof(Vertex3D) * GConfig.VertexCount, MEMORY_TAG_ARRAY);
    FMFree(GConfig.Indices, sizeof(UInt32) * GConfig.IndexCount, MEMORY_TAG_ARRAY);

    // State->TestGeometry = GeometrySystemGetDefault();

    if (!State->Instance->Initialize(State->Instance))
    {
        FLFATAL("Game Failed to Initalize");
        return false;
    }

    // Call resize once to ensure the proper size has been set.
    State->Instance->OnResize(State->Instance, State->Width, State->Height);

    return true;
}

Bool8 AppRun()
{
    ClockStart(&State->Clock);
    ClockUpdate(&State->Clock);
    State->PreviousTime = State->Clock.Elapsed;
    Float64 RunningTime = 0;
    UInt8 FrameCount = 0;
    Float64 TargetFrameRate = 120.0;
    Float64 TargetFrameSeconds = 1.0 / TargetFrameRate;
    State->IsRunning = true;

    FLDEBUG("%s", FMGetMemoryUsageString());

    while(State->IsRunning)
        {
            if (!PlatformPollEvents())
            {
                State->IsRunning = false;
            }

            if (!State->IsSuspended)
            {
                ClockUpdate(&State->Clock);
                Float64 CurrentTime = State->Clock.Elapsed;
                Float64 DeltaTime = (CurrentTime - State->PreviousTime);
                Float64 FrameStartTime = PlatformGetAbsoluteTime();

                if (!State->Instance->Update(State->Instance, (Float32)DeltaTime))
                {
                    FLFATAL("The Game's Update Function failed! shutting down application");
                    State->IsRunning = false;
                    break;
                }

                // Calls Render Routine
                if (!State->Instance->Render(State->Instance, (Float32)DeltaTime))
                {
                    FLFATAL("The Game's Render Function failed! shutting down application");
                    State->IsRunning = false;
                    break;
                }

                // TODO: Refactor Packet Creation
                RenderPacket Packet;

                GeometryRenderData TestRender;
                TestRender.Geometry = State->TestGeometry;
                Mat4Identity(&TestRender.Model);

                Packet.GeometryCount = 1;
                Packet.Geometries = &TestRender;

                Packet.UIGeometryCount = 0;
                Packet.UIGeometries = 0;

                Packet.DeltaTime = (Float32)DeltaTime;
                
                CrystalDrawFrame(&Packet);

                Float64 FrameEndTime = PlatformGetAbsoluteTime();
                Float64 FrameElapsedTime = FrameEndTime - FrameStartTime;
                RunningTime += FrameElapsedTime;
                Float64 RemainingSeconds = TargetFrameSeconds - FrameElapsedTime;

                if (RemainingSeconds > 0)
                {
                    UInt64 RemainingMiliseconds = (UInt64)(RemainingSeconds * 1000);

                    Bool8 LimitFrames = false;
                    if (RemainingMiliseconds > 0 && LimitFrames)
                    {
                        PlatformSleep(RemainingMiliseconds - 1);
                    }
                }
                FrameCount++;

                (void)FrameCount;
                (void)RunningTime;

            // NOTE: Input update/state copying should always be handled
            // after any input should be recorded; I.E. before this line.
            // As a safety, input is the last thing to be updated before
            // this frame ends.
            InputUpdate(DeltaTime);

                State->PreviousTime = CurrentTime;
            }
        }

    State->IsRunning = false;

    EventUnregister(EVENT_APP_QUIT, 0, AppOnEvent);

    EventUnregister(EVENT_KEY_DOWN, 0, AppOnKeyEvent);
    EventUnregister(EVENT_KEY_UP, 0, AppOnKeyEvent);

    EventUnregister(EVENT_MOUSE_DOWN, 0, AppOnMouseButtonEvent);
    EventUnregister(EVENT_MOUSE_UP, 0, AppOnMouseButtonEvent);

    EventUnregister(EVENT_RESIZED, 0, AppOnWindowResize);

    EventUnregister(EVENT_DEBUG0, 0, EventOnDebugEvent);

    InputSystemShutdown(State->InputState);

    GeometrySystemShutdown(State->GeometryState);

    MaterialSystemShutdown(State->MaterialState);

    TextureSystemShutdown(State->TextureState);

    ResourceSystemShutdown(State->ResourceState);

    CrystalShutdown();

    PlatformSystemShutdown(&State->PlatformState);

    MemorySystemShutdown(State->MemoryState);

    EventSystemShutdown(State->EventState);

    return true;
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
            State->IsRunning = false;
            return true;
        }
    }
    return false;
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
            return true;
        
        } else if (KeyCode == AKey) {
            // Example on checking for a key
            // FLDEBUG("Explicit - A key pressed!");
        } else {
            // FLTRACE("'%c' key pressed in window.", KeyCode);
        }
    } else if (Code == EVENT_KEY_UP) {
        UInt16 KeyCode = Context.Data.UInt16[0];
        if (KeyCode == BKey) {
            // Example on checking for a key
            // FLDEBUG("Explicit - B key released!");
        } else {
            // FLTRACE("'%c' key released in window.", KeyCode);
        }
    }
    return false;
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
            // FLDEBUG("Explicit - Right mouse button pressed!");
        } else {
            // FLTRACE("Mouse button %d pressed in window.", ButtonCode);
        }
        
    } else if (Code == EVENT_MOUSE_UP)
    {
        UInt16 ButtonCode = Context.Data.UInt16[0];
        
        if (ButtonCode == MouseButtonLeft) {
            // FLDEBUG("Explicit - Left mouse button released!");
        } else {
            // FLTRACE("Mouse button %d released in window.", ButtonCode);
        }
    }
    return false;
}

Bool8 AppOnWindowResize(UInt16 Code, void* Sender, void* Reciever, EventContext Context)
{
    (void)Reciever;
    (void)Sender;
    if (Code == EVENT_RESIZED)
    {
        UInt16 Width = Context.Data.UInt16[0];
        UInt16 Height = Context.Data.UInt16[1];

        if (Width != State->Width || Height != State->Height)
        {
            State->Width = Width;
            State->Height = Height;

            // FLTRACE("Window Resize: %i, %i", Width, Height);

             if (Width == 0 || Height == 0) {
                FLINFO("Window minimized, suspending application.");
                State->IsSuspended = true;
                return true;
            } else {
                if (State->IsSuspended) {
                    FLINFO("Window restored, resuming application.");
                    State->IsSuspended = false;
                }
                State->Instance->OnResize(State->Instance, Width, Height);
                CrystalOnResize(Width, Height);
            }
        }
    }

    return false;
}