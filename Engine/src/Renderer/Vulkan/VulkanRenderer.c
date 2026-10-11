#include "VulkanRenderer.h"

#include "VulkanDef.inl"
#include "VulkanDevice.h"
#include "VulkanSwapchain.h"
#include "VulkanRenderpass.h"
#include "VulkanImage.h"
#include "VulkanCommandBuffer.h"
#include "VulkanBuffer.h"
#include "VulkanUtils.h"

#include "Core/App.h"
#include "Core/Logger.h"
#include "Core/FString.h"
#include "Core/Memory.h"

#include "Math/MathDef.h"

#include "Containers/Vector.h"

#include "VulkanPlatform.h"

// Shaders
#include "Shaders/VulkanMaterialShader.h"
#include "Shaders/VulkanUIShader.h"

#include "Systems/MaterialSystem.h"

static VulkanContext Context;
static UInt32 CachedFramebufferWidth = 0;
static UInt32 CachedFramebufferHeight = 0;
static UInt8 CurrentRenderpass = 0;

VKAPI_ATTR VkBool32 VKAPI_CALL VkDebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT MessageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT MessageTypes,
    const VkDebugUtilsMessengerCallbackDataEXT* CallbackData,
    void* UserData);

Int32 FindMemoryIndex(UInt32 TypeFilter, UInt32 PropertyFlags);
Bool8 CreateBuffers(VulkanContext* Context);

void CreateCommandBuffers(CrystalBackend* Backend);
Bool8 RecreateSwapchain(CrystalBackend* Backend);
void RegenerateFramebuffers();

void UploadDataRange(VulkanContext* Context, VkCommandPool Pool, VkFence Fence, VkQueue Queue, VulkanBuffer* Buffer, UInt64 Offset, UInt64 Size, const void* Data)
{
    VkBufferUsageFlags Flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    VulkanBuffer Staging;
    VulkanBufferCreate(Context, Size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, Flags, true, &Staging);

    VulkanBufferLoadData(Context, &Staging, 0, Size, 0, Data);

    VulkanBufferCopyTo(Context, Pool, Fence, Queue, Staging.Handle, 0, Buffer->Handle, Offset, Size);

    VulkanBufferDestroy(Context, &Staging);
}

void FreeDataRange(VulkanBuffer* Buffer, UInt64 Offset, UInt64 Size)
{
    (void)Buffer;
    (void)Offset;
    (void)Size;
    // Placeholder...
}

Bool8 VulkanRendererInitialize(CrystalBackend* Backend, const char* AppName) 
{
    // Function Pointers
    Context.FindMemoryIndex = FindMemoryIndex;

    // TODO: custom allocator.
    Context.Allocator = 0;

    AppGetWindowSize(&CachedFramebufferWidth, &CachedFramebufferHeight);

    Context.FramebufferWidth = (CachedFramebufferWidth != 0) ? CachedFramebufferWidth : 1280;
    Context.FramebufferHeight = (CachedFramebufferHeight != 0) ? CachedFramebufferHeight : 720;

    // Setup Vulkan instance.
    VkApplicationInfo AppInfo = {0};
    AppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    AppInfo.apiVersion = VK_API_VERSION_1_3;
    AppInfo.pApplicationName = AppName;
    AppInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    AppInfo.pEngineName = "Fractal Engine";
    AppInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);

    const char** RequiredExtensions = VectorCreate(const char**);
    VectorAppend(RequiredExtensions, &VK_KHR_SURFACE_EXTENSION_NAME);
    PlatformGetRequiredExtensions(&RequiredExtensions);

    #if defined(_DEBUG)
    VectorAppend(RequiredExtensions, &VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    FLDEBUG("Required Extensions:");
    UInt32 Length = VectorSize(RequiredExtensions);
    for (UInt32 i = 0; i < Length; ++i)
    {
        FLDEBUG(RequiredExtensions[i]);
    }
    #endif

    const char** RequiredValidationLayers = 0;
    UInt32 RequiredValidationLayersCount = 0;

    #if defined(_DEBUG)
    FLINFO("Validation layers enabled. Enumerating...");

    // The list of validation layers required.
    RequiredValidationLayers = VectorCreate(const char*);
    VectorAppend(RequiredValidationLayers, &"VK_LAYER_KHRONOS_validation");
    RequiredValidationLayersCount = VectorSize(RequiredValidationLayers);

    // Obtain a list of available validation layers
    UInt32 AvailableLayerCount = 0;
    VK_CHECK(vkEnumerateInstanceLayerProperties(&AvailableLayerCount, 0));
    VkLayerProperties* AvailableLayers = VectorReserve(VkLayerProperties, AvailableLayerCount);
    VK_CHECK(vkEnumerateInstanceLayerProperties(&AvailableLayerCount, AvailableLayers));

    // Verify all required layers are available.
    for (UInt32 i = 0; i < RequiredValidationLayersCount; ++i) {
        FLINFO("Searching for layer: %s...", RequiredValidationLayers[i]);
        Bool8 Found = false;
        for (UInt32 j = 0; j < AvailableLayerCount; ++j) {
            if (StringsEqual(RequiredValidationLayers[i], AvailableLayers[j].layerName)) {
                Found = true;
                FLINFO("Found.");
                break;
            }
        }

        if (!Found) {
            FLFATAL("Required validation layer is missing: %s", RequiredValidationLayers[i]);
            VectorDestroy(RequiredExtensions);
            VectorDestroy(RequiredValidationLayers);
            VectorDestroy(AvailableLayers);
            return false;
        }
    }
    FLINFO("All required validation layers are present.");
    #endif

    #if defined(_DEBUG)
        // 1. Populate debug messenger parameters FIRST
        VkDebugUtilsMessengerCreateInfoEXT DebugCreateInfo = {0};
        DebugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        DebugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT   |
                                          VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT; 
                                          // |
                                          // VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
        DebugCreateInfo.messageType     = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT    |
                                          VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                          VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        DebugCreateInfo.pfnUserCallback = VkDebugCallback;
    #endif

        VkInstanceCreateInfo CreateInfo = {0};
        CreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        CreateInfo.pApplicationInfo = &AppInfo;
        CreateInfo.enabledExtensionCount = VectorSize(RequiredExtensions);
        CreateInfo.ppEnabledExtensionNames = RequiredExtensions;
        CreateInfo.enabledLayerCount = RequiredValidationLayersCount;
        CreateInfo.ppEnabledLayerNames = RequiredValidationLayers;

    #if defined(_DEBUG)
        // 2. Chain into pNext to catch errors DURING vkCreateInstance and vkDestroyInstance
        CreateInfo.pNext = &DebugCreateInfo;
    #endif

        VK_CHECK(vkCreateInstance(&CreateInfo, Context.Allocator, &Context.Instance));
        FLINFO("Vulkan Instance created");

        VectorDestroy(RequiredExtensions);

    #if defined(_DEBUG)
        if (RequiredValidationLayers) {
            VectorDestroy(RequiredValidationLayers);
        }
        if (AvailableLayers) {
            VectorDestroy(AvailableLayers);
        }

        // 3. Register the persistent messenger for all API calls AFTER instance creation
        PFN_vkCreateDebugUtilsMessengerEXT Function =
            (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(Context.Instance, "vkCreateDebugUtilsMessengerEXT");
        FASSERT_MSG(Function, "Failed to create debug messenger!");
        VK_CHECK(Function(Context.Instance, &DebugCreateInfo, Context.Allocator, &Context.DebugMessenger));
        FLDEBUG("Vulkan debugger created.");
    #endif

    // Surface
    FLDEBUG("Creating Vulkan surface...");
    if (!PlatformCreateVulkanSurface(&Context)) {
        FLERROR("Failed to create platform surface!");
        return false;
    }
    FLDEBUG("Vulkan surface created.");

    FLDEBUG("Creating devices...");

    // Device Creation
    if (!VulkanDeviceCreate(&Context))
    {
        FLERROR("Failed to create Devices!");
        return false;
    }

    VulkanSwapchainCreate(&Context, Context.FramebufferWidth, Context.FramebufferHeight, &Context.Swapchain);

    // World Render pass
    VulkanRenderpassCreate(
        &Context,
        &Context.MainRenderpass, 
        (Vec4){.x = 0, .y = 0, .z = Context.FramebufferWidth, .w = Context.FramebufferHeight,},
        (Vec4){.r = 0.01f, .g = 0.01f, .b = 0.01f, .a = 1.0f},
        1.0f,
        0,
        RENDERPASS_CLEAR_COLOR_BUFFER_FLAG | RENDERPASS_CLEAR_DEPTH_BUFFER_FLAG | RENDERPASS_CLEAR_STENCIL_BUFFER_FLAG,
        false,
        true);

    // UI render pass
    VulkanRenderpassCreate(
        &Context,
        &Context.UIRenderpass, 
        (Vec4){.x = 0, .y = 0, .z = Context.FramebufferWidth, .w = Context.FramebufferHeight,},
        (Vec4){.r = 0.0f, .g = 0.0f, .b = 0.0f, .a = 0.0f},
        1.0f,
        0,
        RENDERPASS_CLEAR_NONE_FLAG,
        true,
        false);

    RegenerateFramebuffers();

    FLDEBUG("Creating Command Buffers...");
    CreateCommandBuffers(Backend);

    // Sync Objects Allocation
    Context.ImageAvailableSemaphores = VectorReserve(VkSemaphore, Context.Swapchain.MaxFramesInFlight);
    Context.QueueCompleteSemaphores = VectorReserve(VkSemaphore, Context.Swapchain.ImageCount);

    VkSemaphoreCreateInfo SemaphoreCreateInfo = {0};
    SemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    for (UInt32 i = 0; i < Context.Swapchain.MaxFramesInFlight; ++i) {
        vkCreateSemaphore(Context.Device.LogicalDevice, &SemaphoreCreateInfo, Context.Allocator, &Context.ImageAvailableSemaphores[i]);

        VkFenceCreateInfo FenceCreateInfo = {0};
        FenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        FenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        VK_CHECK(vkCreateFence(Context.Device.LogicalDevice, &FenceCreateInfo, Context.Allocator, &Context.InFlightFences[i]));
    }

    for (UInt32 i = 0; i < Context.Swapchain.ImageCount; ++i)
    {
        vkCreateSemaphore(Context.Device.LogicalDevice, &SemaphoreCreateInfo, Context.Allocator, &Context.QueueCompleteSemaphores[i]);
        Context.ImagesInFlight[i] = 0;
    }

    // Create Builtin Shaders
    if (!VulkanMaterialShaderCreate(&Context, &Context.MaterialShader))
    {
        FLERROR("Error loading Builtin Basic Lighting Shader.");
        return false;
    }

    if (!VulkanUIShaderCreate(&Context, &Context.UIShader))
    {
        FLERROR("Error loading Builtin Basic UI Shader.");
        return false;
    }

    CreateBuffers(&Context);

    for (UInt32 i = 0; i < VulkanMaxGeometryCount; ++i)
    {
        Context.Geometries[i].ID = InvalidID;
    }

    FLINFO("Vulkan renderer initialized successfully");
    return true;
}


void VulkanRendererShutdown(CrystalBackend* Backend)
{
    (void)Backend;
    vkDeviceWaitIdle(Context.Device.LogicalDevice);

    VulkanBufferDestroy(&Context, &Context.ObjectVertexBuffer);
    VulkanBufferDestroy(&Context, &Context.ObjectIndexBuffer);

    VulkanMaterialShaderDestroy(&Context, &Context.MaterialShader);
    VulkanUIShaderDestroy(&Context, &Context.UIShader);
   
    for (UInt32 i = 0; i < Context.Swapchain.MaxFramesInFlight; ++i) {
        if (Context.ImageAvailableSemaphores[i]) {
            vkDestroySemaphore(
                Context.Device.LogicalDevice,
                Context.ImageAvailableSemaphores[i],
                Context.Allocator);
        }
        vkDestroyFence(Context.Device.LogicalDevice, Context.InFlightFences[i], Context.Allocator);
    }

    for (UInt32 i = 0; i < Context.Swapchain.ImageCount; ++i) {
        if (Context.QueueCompleteSemaphores[i]) {
            vkDestroySemaphore(Context.Device.LogicalDevice, Context.QueueCompleteSemaphores[i], Context.Allocator);
        }
    }

    VectorDestroy(Context.ImageAvailableSemaphores);
    Context.ImageAvailableSemaphores = 0;

    VectorDestroy(Context.QueueCompleteSemaphores);
    Context.QueueCompleteSemaphores = 0;




    for (UInt32 i = 0; i < Context.Swapchain.ImageCount; ++i)
    {
        if (Context.GraphicsCommandBuffers[i].Handle)
        {
            VulkanCommandBufferFree(
                &Context,
                Context.Device.GraphicsCommandPool,
                &Context.GraphicsCommandBuffers[i]);
            Context.GraphicsCommandBuffers[i].Handle = 0;
        }
    }
    VectorDestroy(Context.GraphicsCommandBuffers);
    Context.GraphicsCommandBuffers = 0;

    for (UInt32 i = 0; i < Context.Swapchain.ImageCount; ++i)
    {
        vkDestroyFramebuffer(Context.Device.LogicalDevice, Context.WorldFramebuffers[i], Context.Allocator);
        vkDestroyFramebuffer(Context.Device.LogicalDevice, Context.Swapchain.Framebuffers[i], Context.Allocator);
    }

    VulkanRenderpassDestroy(&Context, &Context.UIRenderpass);
    VulkanRenderpassDestroy(&Context, &Context.MainRenderpass);

    VulkanSwapchainDestroy(&Context, &Context.Swapchain);

    FLDEBUG("Destroying Vulkan device");
    VulkanDeviceDestroy(&Context);

    FLDEBUG("Destroying Vulkan Surface");
    if (Context.Surface)
    {
        vkDestroySurfaceKHR(Context.Instance, Context.Surface, Context.Allocator);
        Context.Surface = 0;
    }

    FLDEBUG("Destroying Debugger...");
    if (Context.DebugMessenger)
    {
        PFN_vkDestroyDebugUtilsMessengerEXT Function = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(Context.Instance, "vkDestroyDebugUtilsMessengerEXT");
        Function(Context.Instance, Context.DebugMessenger, Context.Allocator);
    }

    FLDEBUG("Destroying Instance...");
    vkDestroyInstance(Context.Instance, Context.Allocator);
}

void VulkanRendererOnResized(CrystalBackend* Backend, UInt16 Width, UInt16 Height)
{
    (void)Backend;

    CachedFramebufferWidth = Width;
    CachedFramebufferHeight = Height;
    
    Context.FramebufferCurrentGeneration++;

    // FLTRACE("Vulkan Renderer Backend->Resized: Width/Height: %i/%i", Width, Height);
}

Bool8 VulkanRendererBeginFrame(CrystalBackend* Backend, Float32 DeltaTime)
{
    (void)Backend;

    Context.FrameDeltaTime = DeltaTime;

    VulkanDevice* Device = &Context.Device;

    if (Context.RecreateSwapchain) {
        VkResult Result = vkDeviceWaitIdle(Device->LogicalDevice);
        if (!VulkanResultIsSuccess(Result)) {
            FLERROR("VulkanRendererBeginFrame vkDeviceWaitIdle (1) failed: '%s'", VulkanResultString(Result, true));
            return false;
        }
        FLDEBUG("Recreating swapchain, booting...");
        return false;
    }

    if (Context.FramebufferCurrentGeneration != Context.FramebufferLastGeneration) {
        VkResult Result = vkDeviceWaitIdle(Device->LogicalDevice);
        if (!VulkanResultIsSuccess(Result)) {
            FLERROR("VulkanRendererBeginFrame vkDeviceWaitIdle (2) failed: '%s'", VulkanResultString(Result, true));
            return false;
        }

        if(!RecreateSwapchain(Backend)) {
            return false;
        }

        FLDEBUG("Resized, booting...");
        return false;
    }

    VkResult Result = vkWaitForFences(Context.Device.LogicalDevice, 1, &Context.InFlightFences[Context.CurrentFrame], true, UINT64_MAX);
    if (!VulkanResultIsSuccess(Result)) {
        FLERROR("In-Flight Fence wait Failiure: Error: %s", VulkanResultString(Result, true));
        return false;
    }

     if (!VulkanSwapchainAcquireNextImageIndex(
            &Context,
            &Context.Swapchain,
            UINT64_MAX,
            Context.ImageAvailableSemaphores[Context.CurrentFrame],
            0,
            &Context.ImageIndex)) {
        return false;
    }

    // Begin recording the active command buffer
    VulkanCommandBuffer* CommandBuffer = &Context.GraphicsCommandBuffers[Context.ImageIndex];
    VulkanCommandBufferReset(CommandBuffer);
    VulkanCommandBufferBegin(CommandBuffer, false, false, false);

    // Setup dynamic state viewports
    VkViewport Viewport;
    Viewport.x = 0.0f;
    Viewport.y = (Float32)Context.FramebufferHeight;
    Viewport.width = (Float32)Context.FramebufferWidth;
    Viewport.height = -(Float32)Context.FramebufferHeight;
    Viewport.minDepth = 0.0f;
    Viewport.maxDepth = 1.0f;

    VkRect2D Scissor;
    Scissor.offset.x = 0;
    Scissor.offset.y = 0;
    Scissor.extent.width = Context.FramebufferWidth;
    Scissor.extent.height = Context.FramebufferHeight;

    vkCmdSetViewport(CommandBuffer->Handle, 0, 1, &Viewport);
    vkCmdSetScissor(CommandBuffer->Handle, 0, 1, &Scissor);

    Context.MainRenderpass.RenderArea.z = (Float32)Context.FramebufferWidth;
    Context.MainRenderpass.RenderArea.w = (Float32)Context.FramebufferHeight;

    return true;
}

Bool8 VulkanRendererEndFrame(CrystalBackend* Backend, Float32 DeltaTime)
{
    (void)DeltaTime;
    (void)Backend;

    VulkanCommandBuffer* CommandBuffer = &Context.GraphicsCommandBuffers[Context.ImageIndex];

    VulkanCommandBufferEnd(CommandBuffer);

    if (Context.ImagesInFlight[Context.ImageIndex] != VK_NULL_HANDLE) {  // was frame
        VkResult Result = vkWaitForFences(Context.Device.LogicalDevice, 1, Context.ImagesInFlight[Context.ImageIndex], true, UINT64_MAX);
        if (!VulkanResultIsSuccess(Result))
        {
            FLFATAL("VkFenceWait Error: %s", VulkanResultString(Result, true));
        }
    }

    // Mark the image fence as in-use by this frame.
    Context.ImagesInFlight[Context.ImageIndex] = &Context.InFlightFences[Context.CurrentFrame];

    // Reset the fence for use on the next frame
    VK_CHECK(vkResetFences(Context.Device.LogicalDevice, 1, &Context.InFlightFences[Context.CurrentFrame]));

    VkSubmitInfo SubmitInfo = {0};
    SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    SubmitInfo.commandBufferCount = 1;
    SubmitInfo.pCommandBuffers = &CommandBuffer->Handle;

    SubmitInfo.signalSemaphoreCount = 1;
    SubmitInfo.pSignalSemaphores = &Context.QueueCompleteSemaphores[Context.ImageIndex];

    SubmitInfo.waitSemaphoreCount = 1;
    SubmitInfo.pWaitSemaphores = &Context.ImageAvailableSemaphores[Context.CurrentFrame];

    VkPipelineStageFlags Flags[1] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    SubmitInfo.pWaitDstStageMask = Flags;

    VkResult Result = vkQueueSubmit(
        Context.Device.GraphicsQueue,
        1,
        &SubmitInfo,
        Context.InFlightFences[Context.CurrentFrame]);
        
    if (Result != VK_SUCCESS) {
        FLERROR("vkQueueSubmit failed with Result: %s", VulkanResultString(Result, true));
        return false;
    }

    VulkanCommandBufferUpdateSubmitted(CommandBuffer);

    VulkanSwapchainPresent(
        &Context,
        &Context.Swapchain,
        Context.Device.GraphicsQueue,
        Context.Device.PresentQueue,
        Context.QueueCompleteSemaphores[Context.ImageIndex],
        Context.ImageIndex);

    return true;
}

void VulkanRendererUpdateGlobalWorldState(Mat4 Projection, Mat4 View, Vec3 ViewPosition, Vec4 AmbientColor, Int32 Mode)
{
    // VulkanCommandBuffer* CommandBuffer = &Context.GraphicsCommandBuffers[Context.ImageIndex];

    (void)ViewPosition;
    (void)AmbientColor;
    (void)Mode;

    VulkanMaterialShaderUse(&Context, &Context.MaterialShader);

    Context.MaterialShader.GlobalUBO.Projection = Projection;
    Context.MaterialShader.GlobalUBO.View = View;

    // TODO: Other Properties

    VulkanMaterialShaderUpdateGlobalState(&Context, &Context.MaterialShader, Context.FrameDeltaTime);
}

void VulkanRendererUpdateGlobalUIState(Mat4 Projection, Mat4 View, Int32 Mode)
{
    // VulkanCommandBuffer* CommandBuffer = &Context.GraphicsCommandBuffers[Context.ImageIndex];
    (void)Mode;

    VulkanUIShaderUse(&Context, &Context.UIShader);

    Context.UIShader.GlobalUBO.Projection = Projection;
    Context.UIShader.GlobalUBO.View = View;

    // TODO: Other Properties

    VulkanUIShaderUpdateGlobalState(&Context, &Context.UIShader, Context.FrameDeltaTime);
}

Bool8 VulkanRendererBeginRenderpass(struct CrystalBackend* Backend, UInt8 RenderpassID)
{
    (void)Backend;
    CurrentRenderpass = RenderpassID;
    
    VulkanRenderpass* Renderpass = 0;
    VkFramebuffer Framebuffer = 0;
    VulkanCommandBuffer* CommandBuffer = &Context.GraphicsCommandBuffers[Context.ImageIndex];

    switch (RenderpassID)
    {
        case BUILTIN_RENDERPASS_WORLD:
            Renderpass = &Context.MainRenderpass;
            Framebuffer = Context.WorldFramebuffers[Context.ImageIndex];
            break;

        case BUILTIN_RENDERPASS_UI:
            Renderpass = &Context.UIRenderpass;
            Framebuffer = Context.Swapchain.Framebuffers[Context.ImageIndex];
            break;

        default:
            FLERROR("VulkanRendererBeginRenderpass called on unreconized Renderpass ID: %#02x", RenderpassID);
            return false;
    }

    VulkanRenderpassBegin(CommandBuffer, Renderpass, Framebuffer);

    switch (RenderpassID)
    {
        case BUILTIN_RENDERPASS_WORLD:
            VulkanMaterialShaderUse(&Context, &Context.MaterialShader);
            break;

        case BUILTIN_RENDERPASS_UI:
            VulkanUIShaderUse(&Context, &Context.UIShader);
            break;
    }

    return true;
}

Bool8 VulkanRendererEndRenderpass(struct CrystalBackend* Backend, UInt8 RenderpassID)
{
    (void)Backend;
    VulkanRenderpass* Renderpass = 0;
    VulkanCommandBuffer* CommandBuffer = &Context.GraphicsCommandBuffers[Context.ImageIndex];

    switch (RenderpassID)
    {
        case BUILTIN_RENDERPASS_WORLD:
            Renderpass = &Context.MainRenderpass;
            break;

        case BUILTIN_RENDERPASS_UI:
            Renderpass = &Context.UIRenderpass;
            break;

        default:
            FLERROR("VulkanRendererEndRenderpass called on unreconized Renderpass ID: %#02x", RenderpassID);
            return false;
    }

    VulkanRenderpassEnd(CommandBuffer, Renderpass);
    return true;
}

void VulkanRendererCreateTexture(Texture* Texture, const UInt8* Pixels)
{
    // TODO: Allocator for this bro
    Texture->InternalData = (VulkanTextureData*)FMAllocate(sizeof(VulkanTextureData), MEMORY_TAG_TEXTURE);
    VulkanTextureData* Data = (VulkanTextureData*)Texture->InternalData;
    VkDeviceSize ImageSize = Texture->Width * Texture->Height * Texture->ChannelCount;

    // NOTE: assumes 8 bits p[er channel
    VkFormat ImageFormat = VK_FORMAT_R8G8B8A8_UNORM;

    VkBufferUsageFlags Usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    VkMemoryPropertyFlags MemoryPropertyFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    VulkanBuffer Staging;
    VulkanBufferCreate(&Context, ImageSize, Usage, MemoryPropertyFlags, true, &Staging);

    VulkanBufferLoadData(&Context, &Staging, 0, ImageSize, 0, Pixels);

    // NOTE: Lots of assumptions, will require config driven options for different texture types...
    VulkanImageCreate(
        &Context,
        VK_IMAGE_TYPE_2D,
        Texture->Width,
        Texture->Height,
        ImageFormat,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        true,
        VK_IMAGE_ASPECT_COLOR_BIT,
        &Data->Image);

    VulkanCommandBuffer TempBuffer;
    VkCommandPool Pool = Context.Device.GraphicsCommandPool;
    VkQueue Queue = Context.Device.GraphicsQueue;
    VulkanCommandBufferAllocateAndBeginSingleUse(&Context, Pool, &TempBuffer);

    VulkanImageTransitionLayout(
        &Context,
        &TempBuffer,
        &Data->Image,
        ImageFormat,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    VulkanImageCopyFromBuffer(&Context, &Data->Image, Staging.Handle, &TempBuffer);

    VulkanImageTransitionLayout(
        &Context,
        &TempBuffer,
        &Data->Image,
        ImageFormat,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    VulkanCommandBufferEndSingleUse(&Context, Pool, &TempBuffer, Queue);

    VulkanBufferDestroy(&Context, &Staging);

    // Float32 SamplerAnisotropy = FCLAMP(16.0f, 1.0f, Context.Device.Properties.limits.maxSamplerAnisotropy);

    VkSamplerCreateInfo SamplerInfo = {0};
    SamplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    // TODO: Should be config driven
    SamplerInfo.magFilter = VK_FILTER_LINEAR;
    SamplerInfo.minFilter = VK_FILTER_LINEAR;
    SamplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    SamplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    SamplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    SamplerInfo.anisotropyEnable = VK_TRUE;
    SamplerInfo.maxAnisotropy = 16.0f;
    SamplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    SamplerInfo.unnormalizedCoordinates = VK_FALSE;
    SamplerInfo.compareEnable = VK_FALSE;
    SamplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    SamplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    SamplerInfo.mipLodBias = 0.0f;
    SamplerInfo.minLod = 0.0f;
    SamplerInfo.maxLod = 0.0f;

    VkResult Result = vkCreateSampler(Context.Device.LogicalDevice, &SamplerInfo, Context.Allocator, &Data->Sampler);

    if (!VulkanResultIsSuccess(Result))
    {
        FLERROR("Error creating texture sampler: %s", VulkanResultString(Result, true));
        return;
    }

    Texture->Generation++;
}

void VulkanRendererDestroyTexture(Texture* Texture)
{
    vkDeviceWaitIdle(Context.Device.LogicalDevice);

    VulkanTextureData* Data = (VulkanTextureData*)Texture->InternalData;

    if (Data)
    {
        VulkanImageDestroy(&Context, &Data->Image);
        FMZeroMemory(&Data->Image, sizeof(VulkanImage));
        vkDestroySampler(Context.Device.LogicalDevice, Data->Sampler, Context.Allocator);
        Data->Sampler = 0;
        
        FMFree(Texture->InternalData, sizeof(VulkanTextureData), MEMORY_TAG_TEXTURE);
    }

    FMZeroMemory(Texture, sizeof(struct Texture));
}

Bool8 VulkanRendererCreateMaterial(struct Material* Material)
{
    if (Material)
    {
        if (!VulkanMaterialShaderAcquireResources(&Context, &Context.MaterialShader, Material))
        {
            FLERROR("VulkanRendererCreateMaterial - failed to acquire shader resources");
            return false;
        }

        FLTRACE("Crystal: Material Created");
        return true;
    }

    FLERROR("VulkanRendererCreateMaterial called with nullptr, creation failed");
    return false;
}

void VulkanRendererDestroyMaterial(struct Material* Material)
{
    if (Material) {
        if (Material->InternalID != InvalidID) {
            VulkanMaterialShaderReleaseResources(&Context, &Context.MaterialShader, Material);
        } else {
            FLWARN("VulkanRendererDestroyMaterial called with InternalID = InvalidId. Nothing was done");
        }
    } else {
        FLWARN("VulkanRendererDestroyMaterial called with nullptr, Nothing was done");
    }

}

Bool8 VulkanRendererCreateGeometry(struct Geometry* Geometry, UInt32 VertexCount, const Vertex3D* Vertices, UInt32 IndexCount, const UInt32* Indices)
{
    if (!VertexCount || !Vertices)
    {
        FLERROR("VulkanRendererCreateGeometry requires vertex data, none was supplied. VertexCount = %d, Vertices = %p", VertexCount, Vertices);
        return false;
    }

    Bool8 IsReupload = Geometry->InternalID != InvalidID;
    VulkanGeometryData OldRange;

    VulkanGeometryData* InternalData = 0;
    if (IsReupload)
    {
        InternalData = &Context.Geometries[Geometry->InternalID];

        OldRange.IndexBufferOffset = InternalData->IndexBufferOffset;
        OldRange.IndexCount = InternalData->IndexCount;
        OldRange.IndexSize = InternalData->IndexSize;
        OldRange.VertexBufferOffset = InternalData->VertexBufferOffset;
        OldRange.VertexCount = InternalData->VertexCount;
        OldRange.VertexSize = InternalData->VertexSize;
    } else {
        for (UInt32 i = 0; i < VulkanMaxGeometryCount; ++i)
        {
            Geometry->InternalID = i;
            Context.Geometries[i].ID = i;
            InternalData = &Context.Geometries[i];
            break;
        }
    }

    if (!InternalData)
    {
        FLFATAL("VulkanRendererCreateGeometry failed to find a free index, adjust config for more");
        return false;
    }

    VkCommandPool Pool = Context.Device.GraphicsCommandPool;
    VkQueue Queue = Context.Device.GraphicsQueue;

    InternalData->VertexBufferOffset = Context.GeometryVertexOffset;
    InternalData->VertexCount = VertexCount;
    InternalData->VertexSize = sizeof(Vertex3D) * VertexCount;
    UploadDataRange(&Context, Pool, 0, Queue, &Context.ObjectVertexBuffer, InternalData->VertexBufferOffset, InternalData->VertexSize, Vertices);
    Context.GeometryIndexOffset += InternalData->VertexSize;

    if (IndexCount && Indices)
    {
        InternalData->IndexBufferOffset = Context.GeometryIndexOffset;
        InternalData->IndexCount = IndexCount;
        InternalData->IndexSize = sizeof(UInt32) * IndexCount;
        UploadDataRange(&Context, Pool, 0, Queue, &Context.ObjectIndexBuffer, InternalData->IndexBufferOffset, InternalData->IndexSize, Indices);
        Context.GeometryIndexOffset += InternalData->VertexSize;
    }

    if (InternalData->Generation == InvalidID) {
        InternalData->Generation = 0;
    } else {
        InternalData->Generation++;
    }

    if (IsReupload)
    {
        FreeDataRange(&Context.ObjectVertexBuffer, OldRange.VertexBufferOffset, OldRange.VertexSize);

        if (OldRange.IndexSize > 0)
        {
            FreeDataRange(&Context.ObjectIndexBuffer, OldRange.IndexBufferOffset, OldRange.IndexSize);
        }
    }

    return true;
}

void VulkanRendererDestroyGeometry(struct Geometry* Geometry)
{
    if (Geometry && Geometry->InternalID != InvalidID)
    {
        vkDeviceWaitIdle(Context.Device.LogicalDevice);
        VulkanGeometryData* InternalData = &Context.Geometries[Geometry->InternalID];

        FreeDataRange(&Context.ObjectVertexBuffer, InternalData->VertexBufferOffset, InternalData->VertexSize);

        if (InternalData->IndexSize > 0)
        {
            FreeDataRange(&Context.ObjectIndexBuffer, InternalData->IndexBufferOffset, InternalData->IndexSize);
        }

        FMZeroMemory(InternalData, sizeof(VulkanGeometryData));
        InternalData->ID = InvalidID;
        InternalData->Generation = InvalidID;
    }
}

void VulkanRendererDrawGeometry(GeometryRenderData Data)
{
    if (Data.Geometry && Data.Geometry->InternalID == InvalidID) return;

    VulkanGeometryData* BufferData = &Context.Geometries[Data.Geometry->InternalID];
    VulkanCommandBuffer* CommandBuffer = &Context.GraphicsCommandBuffers[Context.ImageIndex];

    // Needs check
    VulkanMaterialShaderUse(&Context, &Context.MaterialShader);

    VulkanMaterialShaderSetModel(&Context, &Context.MaterialShader, Data.Model);
    Material* M = 0;
    if (Data.Geometry->Material) {
        M = Data.Geometry->Material;
    } else {
        M = MaterialSystemGetDefault();
    }
    VulkanMaterialShaderApplyMaterial(&Context, &Context.MaterialShader, M);

    VkDeviceSize Offsets[1] = {BufferData->VertexBufferOffset};
    vkCmdBindVertexBuffers(CommandBuffer->Handle, 0, 1, &Context.ObjectVertexBuffer.Handle, (VkDeviceSize*)Offsets);

    if (BufferData->IndexCount > 0)
    {
    vkCmdBindIndexBuffer(CommandBuffer->Handle, Context.ObjectIndexBuffer.Handle, BufferData->IndexBufferOffset, VK_INDEX_TYPE_UINT32);

    vkCmdDrawIndexed(CommandBuffer->Handle, BufferData->IndexCount, 1, 0, 0, 0);
    } else {
        vkCmdDraw(CommandBuffer->Handle, BufferData->VertexCount, 1, 0, 0);
    }

}

VKAPI_ATTR VkBool32 VKAPI_CALL VkDebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT MessageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT MessageTypes,
    const VkDebugUtilsMessengerCallbackDataEXT* CallbackData,
    void* UserData) {
    (void)MessageTypes;
    (void)UserData;
    switch (MessageSeverity) {
        default:
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            FLERROR("%s", CallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            FLWARN("%s", CallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            FLINFO("%s", CallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            FLTRACE("%s", CallbackData->pMessage);
            break;
    }
    return VK_FALSE;
}

Int32 FindMemoryIndex(UInt32 TypeFilter, UInt32 PropertyFlags)
{
    VkPhysicalDeviceMemoryProperties MemoryProperties = {0};
    vkGetPhysicalDeviceMemoryProperties(Context.Device.PhysicalDevice, &MemoryProperties);

    for (UInt32 i = 0; i < MemoryProperties.memoryTypeCount; ++i)
    {
        if (TypeFilter & (1 << i) && (MemoryProperties.memoryTypes[i].propertyFlags & PropertyFlags) == PropertyFlags)
        {
            return i;
        }
    }

    FLWARN("Unable to find sutiable memory type!");
    return -1;
}

void CreateCommandBuffers(CrystalBackend* Backend)
{
    (void)Backend;

    if (!Context.GraphicsCommandBuffers) {
        Context.GraphicsCommandBuffers = VectorReserve(VulkanCommandBuffer, Context.Swapchain.ImageCount);
        for (UInt32 i = 0; i < Context.Swapchain.ImageCount; ++i)
        {
            FMZeroMemory(&Context.GraphicsCommandBuffers[i], sizeof(VulkanCommandBuffer));
        }
    }

    for (UInt32 i = 0; i < Context.Swapchain.ImageCount; ++i) {
        if (Context.GraphicsCommandBuffers[i].Handle) {
            VulkanCommandBufferFree(
                &Context,
                Context.Device.GraphicsCommandPool,
                &Context.GraphicsCommandBuffers[i]);
        }
        
        VulkanCommandBufferAllocate(
            &Context,
            Context.Device.GraphicsCommandPool,
            true,
            &Context.GraphicsCommandBuffers[i]);
    }

    FLDEBUG("Vulkan command buffers created successfully.");
}

Bool8 RecreateSwapchain(CrystalBackend* Backend)
{
    // If already being recreated, do not try again.
    if (Context.RecreateSwapchain) {
        FLDEBUG("RecreateSwapchain called when already rebuilding. Booting.");
        return false;
    }

    if (CachedFramebufferWidth == 0 || CachedFramebufferHeight == 0) {
        FLDEBUG("RecreateSwapchain called with zero dimensions. Booting.");
        return false;
    }

    // Detect if the window is too small to be drawn to
    if (Context.FramebufferWidth == 0 || Context.FramebufferHeight == 0) {
        FLDEBUG("RecreateSwapchain called with zero dimensions. Booting.");
        return false;
    }

    // Mark as recreating if the dimensions are valid.
    Context.RecreateSwapchain = true;

    vkDeviceWaitIdle(Context.Device.LogicalDevice);

    for (UInt32 i = 0; i < Context.Swapchain.ImageCount; ++i) {
        Context.ImagesInFlight[i] = 0;
    }

    VulkanDeviceQuerySwapchainSupport(
        Context.Device.PhysicalDevice,
        Context.Surface,
        &Context.Device.SwapchainSupport);

    VulkanDeviceDetectDepthFormat(&Context.Device);

    UInt32 OldImageCount = Context.Swapchain.ImageCount;

    // Free command buffers using the explicit configuration
    for (UInt32 i = 0; i < OldImageCount; ++i) {
        VulkanCommandBufferFree(&Context, Context.Device.GraphicsCommandPool, &Context.GraphicsCommandBuffers[i]);
    }

    // Destroy framebuffers using the explicit configuration
    for (UInt32 i = 0; i < OldImageCount; ++i) {
        vkDestroyFramebuffer(Context.Device.LogicalDevice, Context.WorldFramebuffers[i], Context.Allocator);
        vkDestroyFramebuffer(Context.Device.LogicalDevice, Context.Swapchain.Framebuffers[i], Context.Allocator);
    }

    // Recreate the swapchain layout properties
    VulkanSwapchainRecreate(
        &Context,
        CachedFramebufferWidth,
        CachedFramebufferHeight,
        &Context.Swapchain);

    // Sync the framebuffer size with the cached sizes.
    Context.FramebufferWidth = CachedFramebufferWidth;
    Context.FramebufferHeight = CachedFramebufferHeight;

    // Update Main Renderpass Render Area
    Context.MainRenderpass.RenderArea.x = 0;
    Context.MainRenderpass.RenderArea.y = 0;
    Context.MainRenderpass.RenderArea.z = (Float32)Context.FramebufferWidth;
    Context.MainRenderpass.RenderArea.w = (Float32)Context.FramebufferHeight;

    // Update UI Renderpass Render Area
    Context.UIRenderpass.RenderArea.x = 0;
    Context.UIRenderpass.RenderArea.y = 0;
    Context.UIRenderpass.RenderArea.z = (Float32)Context.FramebufferWidth;
    Context.UIRenderpass.RenderArea.w = (Float32)Context.FramebufferHeight;

    CachedFramebufferWidth = 0;
    CachedFramebufferHeight = 0;

    // Regenerates swapchain and world buffers
    RegenerateFramebuffers();
    CreateCommandBuffers(Backend);

    // Clear the tracking states
    Context.FramebufferLastGeneration = Context.FramebufferCurrentGeneration; 
    Context.RecreateSwapchain = false;

    return true;
}

void RegenerateFramebuffers()
{
    UInt32 ImageCount = Context.Swapchain.ImageCount;
    for (UInt32 i = 0; i < ImageCount; ++i)
    {
        VkImageView WorldAttachments[2] = {Context.Swapchain.Views[i], Context.Swapchain.DepthAttachment.View};
        VkFramebufferCreateInfo FramebufferCreateInfo = {0};
        FramebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        FramebufferCreateInfo.renderPass = Context.MainRenderpass.Handle;
        FramebufferCreateInfo.attachmentCount = 2;
        FramebufferCreateInfo.pAttachments = WorldAttachments;
        FramebufferCreateInfo.width = Context.FramebufferWidth;
        FramebufferCreateInfo.height = Context.FramebufferHeight;
        FramebufferCreateInfo.layers = 1;

        VK_CHECK(vkCreateFramebuffer(Context.Device.LogicalDevice, &FramebufferCreateInfo, Context.Allocator, &Context.WorldFramebuffers[i]));

        VkImageView UIAttachments[1] = {Context.Swapchain.Views[i]};
        VkFramebufferCreateInfo SwapchainFramebufferCreateInfo = {0};
        SwapchainFramebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        SwapchainFramebufferCreateInfo.renderPass = Context.UIRenderpass.Handle;
        SwapchainFramebufferCreateInfo.attachmentCount = 1;
        SwapchainFramebufferCreateInfo.pAttachments = UIAttachments;
        SwapchainFramebufferCreateInfo.width = Context.FramebufferWidth;
        SwapchainFramebufferCreateInfo.height = Context.FramebufferHeight;
        SwapchainFramebufferCreateInfo.layers = 1;

        VK_CHECK(vkCreateFramebuffer(Context.Device.LogicalDevice, &SwapchainFramebufferCreateInfo, Context.Allocator, &Context.Swapchain.Framebuffers[i]));
    }
}

Bool8 CreateBuffers(VulkanContext* Context)
{
    VkMemoryPropertyFlagBits MemoryPropertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

    // Geometry vertex buffer
    const UInt64 VertexBufferSize = sizeof(Vertex3D) * 1024 * 1024;
    if (!VulkanBufferCreate(
            Context,
            VertexBufferSize,
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            MemoryPropertyFlags,
            true,
            &Context->ObjectVertexBuffer)) {
        FLERROR("Error creating vertex buffer.");
        return false;
    }
    Context->GeometryVertexOffset = 0;

    // Geometry index buffer
    const UInt64 IndexBufferSize = sizeof(UInt32) * 1024 * 1024;
    if (!VulkanBufferCreate(
            Context,
            IndexBufferSize,
            VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            MemoryPropertyFlags,
            true,
            &Context->ObjectIndexBuffer)) {
        FLERROR("Error creating index buffer.");
        return false;
    }
    Context->GeometryIndexOffset = 0;

    return true;
}