#include "CrystalBackend.h"

#include "Vulkan/VulkanRenderer.h"

Bool8 CrystalBackendCreate(CrystalBackendType Type, CrystalBackend* Backend)
{
    if (Type == CRYSTAL_BACKEND_TYPE_VULKAN)
    {
        Backend->Initialize = VulkanRendererInitialize;
        Backend->Shutdown = VulkanRendererShutdown;
        Backend->BeginFrame = VulkanRendererBeginFrame;
        Backend->UpdateGlobalState = VullkanRendererUpdateGlobalState;
        Backend->EndFrame = VulkanRendererEndFrame;
        Backend->Resized = VulkanRendererOnResized;
        
        Backend->UpdateObject = VulkanRendererUpdateObject;

         return false;
     }

    return false;
}

void CrystalBackendDestroy(CrystalBackend* Backend)
{
    Backend->Initialize = 0;
    Backend->Shutdown = 0;
    Backend->BeginFrame = 0;
    Backend->UpdateGlobalState = 0;
    Backend->EndFrame = 0;
    Backend->Resized = 0;

    Backend->UpdateObject = 0;
}