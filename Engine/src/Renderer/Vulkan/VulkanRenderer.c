#include "VulkanRenderer.h"

#include "VulkanDef.inl"
#include "VulkanDevice.h"
#include "VulkanSwapchain.h"

#include "Core/Logger.h"
#include "Core/FString.h"

#include "Containers/Vector.h"

#include "VulkanPlatform.h"

static VulkanContext Context;

VKAPI_ATTR VkBool32 VKAPI_CALL VkDebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT MessageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT MessageTypes,
    const VkDebugUtilsMessengerCallbackDataEXT* CallbackData,
    void* UserData);

Int32 FindMemoryIndex(UInt32 TypeFilter, UInt32 PropertyFlags);

Bool8 VulkanRendererInitialize(CrystalBackend* Backend, const char* AppName, struct PlatformState* PlatformState) {
    
    (void)Backend;
    (void)PlatformState;

    // Function Pointers
    Context.FindMemoryIndex = FindMemoryIndex;

    // TODO: custom allocator.
    Context.Allocator = 0;

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
                                          VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                          VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
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
    if (!PlatformCreateVulkanSurface(PlatformState, &Context)) {
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

    FLINFO("Vulkan renderer initialized successfully");
    return true;
}


void VulkanRendererShutdown(CrystalBackend* Backend)
{
    (void)Backend;
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
    (void)Width;
    (void)Height;
}

Bool8 VulkanRendererBeginFrame(CrystalBackend* Backend, Float32 DeltaTime)
{
    (void)Backend;
    (void)DeltaTime;

    return true;
}

Bool8 VulkanRendererEndFrame(CrystalBackend* Backend, Float32 DeltaTime)
{
    (void)Backend;
    (void)DeltaTime;

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
            return 1;
        }
    }

    FLWARN("Unable to find sutiable memory type!");
    return -1;
}