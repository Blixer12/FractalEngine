#include "RendererBackend.h"

#include "Vulkan/VulkanRenderer.h"

Bool8 RendererBackendCreate(RendererBackendType Type, struct PlatformState* Platform, RendererBackend* Backend)
{
    Backend->Platform = Platform;

    if (Type == RENDERER_BACKEND_TYPE_VULKAN)
    {
        Backend->Initialize = VulkanRendererInitialize;
        Backend->Shutdown = VulkanRendererShutdown;
        Backend->BeginFrame = VulkanrRendererBeginFrame;
        Backend->EndFrame = VulkanrRendererEndFrame;
        Backend->Resized = VulkanRendererOnResized;

        return TRUE;
    }

    return FALSE;
}

void RendererBackendDestroy(RendererBackend* Backend)
{
    Backend->Initialize = 0;
    Backend->Shutdown = 0;
    Backend->BeginFrame = 0;
    Backend->EndFrame = 0;
    Backend->Resized = 0;
}