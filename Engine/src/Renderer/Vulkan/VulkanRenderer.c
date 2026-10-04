#include "VulkanRenderer.h"

#include "VulkanDef.inl"
#include "VulkanDevice.h"
#include "VulkanSwapchain.h"
#include "VulkanRenderpass.h"
#include "VulkanCommandBuffer.h"
#include "VulkanFramebuffer.h"
#include "VulkanFence.h"
#include "VulkanUtils.h"

#include "Core/App.h"
#include "Core/Logger.h"
#include "Core/FString.h"
#include "Core/Memory.h"

#include "Containers/Vector.h"

#include "VulkanPlatform.h"

// Shaders
#include "Shaders/VulkanObjectShader.h"

static VulkanContext Context;
static UInt32 CachedFramebufferWidth = 0;
static UInt32 CachedFramebufferHeight = 0;

VKAPI_ATTR VkBool32 VKAPI_CALL VkDebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT MessageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT MessageTypes,
    const VkDebugUtilsMessengerCallbackDataEXT* CallbackData,
    void* UserData);

Int32 FindMemoryIndex(UInt32 TypeFilter, UInt32 PropertyFlags);

void CreateCommandBuffers(CrystalBackend* Backend);
Bool8 RecreateSwapchain(CrystalBackend* Backend);
void RegenerateFramebuffers(CrystalBackend* Backend, VulkanSwapchain* Swapchain, VulkanRenderpass* Renderpass);


Bool8 VulkanRendererInitialize(CrystalBackend* Backend, const char* AppName) {
    
    (void)Backend;

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
    for (UInt32 i = 0; i < Length; i++)
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
            if (StringCompare(RequiredValidationLayers[i], AvailableLayers[j].layerName)) {
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

    VulkanRenderpassCreate(
        &Context,
        &Context.MainRenderpass, 
        0, 0, Context.FramebufferWidth, Context.FramebufferHeight,
        0.05f, 0.05f, 0.05f, 1.0f, 
        1.0f,
        0);

    Context.Swapchain.Framebuffers = VectorReserve(VulkanFramebuffer, Context.Swapchain.ImageCount);
    RegenerateFramebuffers(Backend, &Context.Swapchain, &Context.MainRenderpass);

    FLDEBUG("Creating Command Buffers...");
    CreateCommandBuffers(Backend);

    // Sync Objects Allocation
    Context.ImageAvailableSemaphores = VectorReserve(VkSemaphore, Context.Swapchain.MaxFramesInFlight);
    Context.QueueCompleteSemaphores = VectorReserve(VkSemaphore, Context.Swapchain.ImageCount);
    Context.InFlightFences = VectorReserve(VulkanFence, Context.Swapchain.MaxFramesInFlight);
    Context.InFlightFenceCount = Context.Swapchain.MaxFramesInFlight;

    VkSemaphoreCreateInfo SemaphoreCreateInfo = {0};
    SemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    for (UInt32 i = 0; i < Context.Swapchain.MaxFramesInFlight; i++) {
        vkCreateSemaphore(Context.Device.LogicalDevice, &SemaphoreCreateInfo, Context.Allocator, &Context.ImageAvailableSemaphores[i]);
        VulkanFenceCreate(&Context, true, &Context.InFlightFences[i]);
    }

    for (UInt32 i = 0; i < Context.Swapchain.ImageCount; i++)
    {
        vkCreateSemaphore(Context.Device.LogicalDevice, &SemaphoreCreateInfo, Context.Allocator, &Context.QueueCompleteSemaphores[i]);
    }

    Context.ImagesInFlight = VectorReserve(VulkanFence, Context.Swapchain.ImageCount);
    for (UInt32 i = 0; i < Context.Swapchain.ImageCount; ++i) {
        Context.ImagesInFlight[i] = 0; 
    }

    // Create Builtin Shaders
    if (!VulkanObjectShaderCreate(&Context, &Context.ObjectShader))
    {
        FLERROR("Error loading Builtin Basic Lighting Shader.");
        return false;
    }

    FLINFO("Vulkan renderer initialized successfully");
    return true;
}


void VulkanRendererShutdown(CrystalBackend* Backend)
{
    (void)Backend;
    vkDeviceWaitIdle(Context.Device.LogicalDevice);

    VulkanObjectShaderDestroy(&Context, &Context.ObjectShader);
   
    for (UInt32 i = 0; i < Context.Swapchain.MaxFramesInFlight; i++) {
        if (Context.ImageAvailableSemaphores[i]) {
            vkDestroySemaphore(
                Context.Device.LogicalDevice,
                Context.ImageAvailableSemaphores[i],
                Context.Allocator);
        }
        VulkanFenceDestroy(&Context, &Context.InFlightFences[i]);
    }

    for (UInt32 i = 0; i < Context.Swapchain.ImageCount; i++) {
        if (Context.QueueCompleteSemaphores[i]) {
            vkDestroySemaphore(Context.Device.LogicalDevice, Context.QueueCompleteSemaphores[i], Context.Allocator);
        }
    }

    VectorDestroy(Context.ImageAvailableSemaphores);
    Context.ImageAvailableSemaphores = 0;

    VectorDestroy(Context.QueueCompleteSemaphores);
    Context.QueueCompleteSemaphores = 0;

    VectorDestroy(Context.ImagesInFlight);
    Context.ImagesInFlight = 0;



    for (UInt32 i = 0; i < Context.Swapchain.ImageCount; i++)
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

    for (UInt32 i = 0; i < Context.Swapchain.ImageCount; i++)
    {
        VulkanFramebufferDestroy(&Context, &Context.Swapchain.Framebuffers[i]);
    }

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
    
    Context.WindowResized = true;

    // FLTRACE("Vulkan Renderer Backend->Resized: Width/Height: %i/%i", Width, Height);
}

Bool8 VulkanRendererBeginFrame(CrystalBackend* Backend, Float32 DeltaTime)
{
    (void)DeltaTime;
    (void)Backend;

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

    if (Context.WindowResized) {
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

    if (!VulkanFenceWait(
        &Context,
        &Context.InFlightFences[Context.CurrentFrame],
        UINT64_MAX)) {
        FLWARN("In-Flight Fence Failiure");
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

    Context.MainRenderpass.W = (Float32)Context.FramebufferWidth;
    Context.MainRenderpass.H = (Float32)Context.FramebufferHeight;

    VulkanRenderpassBegin(
        CommandBuffer,
        &Context.MainRenderpass,
        Context.Swapchain.Framebuffers[Context.ImageIndex].Handle);

    return true;
}

Bool8 VulkanRendererEndFrame(CrystalBackend* Backend, Float32 DeltaTime)
{
    (void)DeltaTime;
    (void)Backend;

    VulkanCommandBuffer* CommandBuffer = &Context.GraphicsCommandBuffers[Context.ImageIndex];

    VulkanRenderpassEnd(CommandBuffer, &Context.MainRenderpass);
    VulkanCommandBufferEnd(CommandBuffer);

    if (Context.ImagesInFlight[Context.ImageIndex] != VK_NULL_HANDLE) {  // was frame
        VulkanFenceWait(
            &Context,
            Context.ImagesInFlight[Context.ImageIndex],
            UINT64_MAX);
    }

    // Mark the image fence as in-use by this frame.
    Context.ImagesInFlight[Context.ImageIndex] = &Context.InFlightFences[Context.CurrentFrame];

    // Reset the fence for use on the next frame
    VulkanFenceReset(&Context, &Context.InFlightFences[Context.CurrentFrame]);

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
        Context.InFlightFences[Context.CurrentFrame].Handle);
        
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
            FLERROR(CallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            FLWARN(CallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            FLINFO(CallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            FLTRACE(CallbackData->pMessage);
            break;
    }
    return VK_FALSE;
}

Int32 FindMemoryIndex(UInt32 TypeFilter, UInt32 PropertyFlags)
{
    VkPhysicalDeviceMemoryProperties MemoryProperties = {0};
    vkGetPhysicalDeviceMemoryProperties(Context.Device.PhysicalDevice, &MemoryProperties);

    for (UInt32 i = 0; i < MemoryProperties.memoryTypeCount; i++)
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
        for (UInt32 i = 0; i < Context.Swapchain.ImageCount; i++)
        {
            FMZeroMemory(&Context.GraphicsCommandBuffers[i], sizeof(VulkanCommandBuffer));
        }
    }

    for (UInt32 i = 0; i < Context.Swapchain.ImageCount; i++) {
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

    // Recreate the swapchain layout properties
    VulkanSwapchainRecreate(
        &Context,
        CachedFramebufferWidth,
        CachedFramebufferHeight,
        &Context.Swapchain);

    // Sync the framebuffer size with the cached sizes.
    Context.FramebufferWidth = CachedFramebufferWidth;
    Context.FramebufferHeight = CachedFramebufferHeight;
    Context.MainRenderpass.W = (Float32)Context.FramebufferWidth;
    Context.MainRenderpass.H = (Float32)Context.FramebufferHeight;
    CachedFramebufferWidth = 0;
    CachedFramebufferHeight = 0;

    // Free command buffers using the explicit configuration
    for (UInt32 i = 0; i < Context.Swapchain.ImageCount; ++i) {
        VulkanCommandBufferFree(&Context, Context.Device.GraphicsCommandPool, &Context.GraphicsCommandBuffers[i]);
    }

    // Destroy framebuffers using the explicit configuration
    for (UInt32 i = 0; i < Context.Swapchain.ImageCount; ++i) {
        VulkanFramebufferDestroy(&Context, &Context.Swapchain.Framebuffers[i]);
    }

    Context.MainRenderpass.X = 0;
    Context.MainRenderpass.Y = 0;
    Context.MainRenderpass.W = (Float32)Context.FramebufferWidth;
    Context.MainRenderpass.H = (Float32)Context.FramebufferHeight;

    RegenerateFramebuffers(Backend, &Context.Swapchain, &Context.MainRenderpass);
    CreateCommandBuffers(Backend);

    // Clear the tracking states
    Context.WindowResized = false; 
    Context.RecreateSwapchain = false;

    return true;
}

void RegenerateFramebuffers(CrystalBackend* Backend, VulkanSwapchain* Swapchain, VulkanRenderpass* Renderpass) {
    (void)Backend;
    for (UInt32 i = 0; i < Swapchain->ImageCount; i++) {
        UInt32 AttachmentCount = 2;
        VkImageView Attachments[] = {
            Swapchain->Views[i],
            Swapchain->DepthAttachment.View};

        VulkanFramebufferCreate(
            &Context,
            Renderpass,
            Context.FramebufferWidth,
            Context.FramebufferHeight,
            AttachmentCount,
            Attachments,
            &Context.Swapchain.Framebuffers[i]);
    }
}