#include "CrystalBackend.h"

#include "Vulkan/VulkanRenderer.h"

Bool8 CrystalBackendCreate(CrystalBackendType Type, CrystalBackend* Backend)
{
    if (Type == CRYSTAL_BACKEND_TYPE_VULKAN)
    {
        Backend->Initialize = VulkanRendererInitialize;
        Backend->Shutdown = VulkanRendererShutdown;

        Backend->BeginFrame = VulkanRendererBeginFrame;
        Backend->EndFrame = VulkanRendererEndFrame;

        Backend->UpdateGlobalWorldState = VulkanRendererUpdateGlobalWorldState;
        Backend->UpdateGlobalUIState = VulkanRendererUpdateGlobalUIState;

        Backend->Resized = VulkanRendererOnResized;
        
        Backend->DrawGeometry = VulkanRendererDrawGeometry;

        Backend->BeginRenderpass = VulkanRendererBeginRenderpass;
        Backend->EndRenderpass = VulkanRendererEndRenderpass;

        Backend->CreateTexture = VulkanRendererCreateTexture;
        Backend->DestroyTexture = VulkanRendererDestroyTexture;

        Backend->CreateMaterial = VulkanRendererCreateMaterial;
        Backend->DestroyMaterial = VulkanRendererDestroyMaterial;

        Backend->CreateGeometry = VulkanRendererCreateGeometry;
        Backend->DestroyGeometry = VulkanRendererDestroyGeometry;

        return true;
     }

    return false;
}

void CrystalBackendDestroy(CrystalBackend* Backend)
{
    Backend->Initialize = 0;
    Backend->Shutdown = 0;

    Backend->BeginFrame = 0;
    Backend->EndFrame = 0;

    Backend->UpdateGlobalWorldState = 0;
    Backend->UpdateGlobalUIState = 0;

    Backend->BeginRenderpass = 0;
    Backend->EndRenderpass = 0;

    Backend->Resized = 0;

    Backend->DrawGeometry = 0;

    Backend->CreateTexture = 0;
    Backend->DestroyTexture = 0;

    Backend->CreateMaterial = 0;
    Backend->DestroyMaterial = 0;

    Backend->CreateGeometry = 0;
    Backend->DestroyGeometry = 0;
}