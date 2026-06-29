#include "VulkanRenderer.h"

#include "VulkanDef.inl"

#include "Core/Logger.h"

static VulkanContext Context;

Bool8 VulkanRendererInitialize(RendererBackend* Backend, const char* AppName, struct PlatformState* Platform)
{
    (void)Platform;
    (void)Backend;
    // TODO: Custom Allocator
    Context.Allocator = 0;

    VkApplicationInfo AppInfo = {VK_STRUCTURE_TYPE_APPLICATION_INFO};
    AppInfo.pNext = 0;
    AppInfo.pApplicationName = AppName;
    AppInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
    AppInfo.pEngineName = "Fractal Engine";
    AppInfo.engineVersion = VK_MAKE_VERSION(0, 0, 1);
    AppInfo.apiVersion = VK_API_VERSION_1_4;

    VkInstanceCreateInfo CreateInfo = {VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
    CreateInfo.pApplicationInfo = &AppInfo;
    CreateInfo.enabledExtensionCount = 0;
    CreateInfo.ppEnabledExtensionNames = 0;
    CreateInfo.enabledLayerCount = 0;
    CreateInfo.ppEnabledLayerNames = 0;

    VkResult Result = vkCreateInstance(&CreateInfo, Context.Allocator, &Context.Instance);
    if(Result != VK_SUCCESS) {
        FLERROR("vkCreateInstance() failed with result: %i", Result);
        return FALSE;
    }

    FLINFO("Vulkan renderer initialized successfully.");
    return TRUE;
    
}
void VulkanRendererShutdown(RendererBackend* Backend)
{
    (void)Backend;
}

void VulkanRendererOnResized(RendererBackend* Backend, UInt16 Width, UInt16 Height)
{
    (void)Backend; (void)Width; (void)Height;
}

Bool8 VulkanrRendererBeginFrame(RendererBackend* Backend, Float32 DeltaTime)
{
    (void)Backend; (void)DeltaTime;
    return TRUE;
}
Bool8 VulkanrRendererEndFrame(RendererBackend* Backend, Float32 DeltaTime)
{
    (void)Backend; (void)DeltaTime;
    return TRUE;
}