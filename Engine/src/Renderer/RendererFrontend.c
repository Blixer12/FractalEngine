#include "RendererFrontend.h"

#include "RendererBackend.h"

#include "Core/Logger.h"
#include "Core/Memory.h"

static RendererBackend* Backend = 0;

Bool8 RendererInitialize(const char* AppName, struct PlatformState* Platform)
{
    Backend = FMAllocate(sizeof(RendererBackend), MEMORY_TAG_RENDERER);

    //TODO: Make this COnfigurable
    RendererBackendCreate(RENDERER_BACKEND_TYPE_VULKAN, Platform, Backend);
    Backend->FrameNumber = 0;

    if (!Backend->Initialize(Backend, AppName, Platform))
    {
        FLFATAL("Renderer Backend failed to Initialize! Shutting down Application");
        return FALSE;
    }

    return TRUE;
}
void RendererShutdown()
{
    Backend->Shutdown(Backend);
    FMFree(Backend, sizeof(Backend), MEMORY_TAG_RENDERER);
}

Bool8 RendererBeginFrame(Float32 DeltaTime)
{
    return Backend->BeginFrame(Backend, DeltaTime);
}

Bool8 RendererEndFrame(Float32 DeltaTime)
{
    Bool8 Result = Backend->EndFrame(Backend, DeltaTime);
    Backend->FrameNumber++;
    return Result;
}

Bool8 RendererDrawFrame(RenderPacket* Packet)
{
    if (RendererBeginFrame(Packet->DeltaTime))
    {

        Bool8 Result = RendererEndFrame(Packet->DeltaTime);

        if (!Result)
        {
            FLERROR("RendererEndFrame Failed! Shutting down Application");
        }
    }

    return TRUE;
}

void RendererOnResize(UInt16 Width, UInt16 Height);