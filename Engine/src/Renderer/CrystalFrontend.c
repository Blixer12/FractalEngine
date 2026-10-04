#include "CrystalFrontend.h"

#include "CrystalBackend.h"

#include "Core/Logger.h"
#include "Core/Memory.h"

static CrystalBackend* StatePtr;

Bool8 CrystalInitialize(UInt64* MemoryRequirement, void* State, const char* AppName)
{
    *MemoryRequirement = sizeof(CrystalBackend);
    if (State == 0) {
        return true;
    }
    StatePtr = State;

    //TODO: Make this Configurable
    CrystalBackendCreate(CRYSTAL_BACKEND_TYPE_VULKAN, StatePtr);
    StatePtr->FrameNumber = 0;

    if (!StatePtr->Initialize(StatePtr, AppName))
    {
        FLFATAL("Renderer Backend failed to Initialize! Shutting down Application");
        return false;
    }

    return true;
}
void CrystalShutdown()
{
    if (StatePtr) {
        StatePtr->Shutdown(StatePtr);
        CrystalBackendDestroy(StatePtr);
        StatePtr = 0;
    }
}

Bool8 CrystalBeginFrame(Float32 DeltaTime)
{
    return StatePtr->BeginFrame(StatePtr, DeltaTime);
}

Bool8 CrystalEndFrame(Float32 DeltaTime)
{
    Bool8 Result = StatePtr->EndFrame(StatePtr, DeltaTime);
    StatePtr->FrameNumber++;
    return Result;
}

void CrystalOnResize(UInt16 Width, UInt16 Height)
{
    if (StatePtr) {
        StatePtr->Resized(StatePtr, Width, Height);
    } else {
        FLERROR("The Crystal backend does not exist to accept resize: %i, %i", Width, Height);
    }
}

Bool8 CrystalDrawFrame(RenderPacket* Packet)
{
    if (CrystalBeginFrame(Packet->DeltaTime))
    {

        Bool8 Result = CrystalEndFrame(Packet->DeltaTime);

        if (!Result)
        {
            FLERROR("CrystalEndFrame Failed! Shutting down Application");
        }
    }

    return true;
}