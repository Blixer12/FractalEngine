#include "CrystalFrontend.h"

#include "CrystalBackend.h"

#include "Core/Logger.h"
#include "Core/Memory.h"

static CrystalBackend* Backend = 0;

Bool8 CrystalInitialize(const char* AppName, struct PlatformState* Platform)
{
    (void)AppName;
    Backend = FMAllocate(sizeof(CrystalBackend), MEMORY_TAG_RENDERER);

    //TODO: Make this Configurable
    CrystalBackendCreate(CRYSTAL_BACKEND_TYPE_VULKAN, Platform, Backend);
    Backend->FrameNumber = 0;

    if (!Backend->Initialize(Backend, AppName, Platform))
    {
        FLFATAL("Renderer Backend failed to Initialize! Shutting down Application");
        return false;
    }

    return true;
}
void CrystalShutdown()
{
    Backend->Shutdown(Backend);
    CrystalBackendDestroy(Backend);
    FMFree(Backend, sizeof(CrystalBackend), MEMORY_TAG_RENDERER);
}

Bool8 CrystalBeginFrame(Float32 DeltaTime)
{
    return Backend->BeginFrame(Backend, DeltaTime);
}

Bool8 CrystalEndFrame(Float32 DeltaTime)
{
    Bool8 Result = Backend->EndFrame(Backend, DeltaTime);
    Backend->FrameNumber++;
    return Result;
}

void CrystalOnResize(UInt16 Width, UInt16 Height)
{
    if (Backend) {
        Backend->Resized(Backend, Width, Height);
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