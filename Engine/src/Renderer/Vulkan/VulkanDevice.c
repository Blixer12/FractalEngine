#include "VulkanDevice.h"

#include "Core/Logger.h"
#include "Core/FString.h"
#include "Core/Memory.h"
#include "Containers/Vector.h"

typedef struct VulkanPhysicalDeviceRequirements {
    Bool8 Graphics;
    Bool8 Present;
    Bool8 Compute;
    Bool8 Transfer;
    
    // NOTE: An array of strings holding requested extension names (e.g., VK_KHR_SWAPCHAIN_EXTENSION_NAME)
    const char** DeviceExtensionNames;
    UInt32 DeviceExtensionCount;

    Bool8 SamplerAnisotropy;
    
    Bool8 DynamicRendering;
    Bool8 Synchronization2;
} VulkanPhysicalDeviceRequirements;

typedef struct VulkanPhysicalDeviceQueueFamilyInfo {
    UInt32 GraphicsFamilyIndex;
    UInt32 PresentFamilyIndex;
    UInt32 ComputeFamilyIndex;
    UInt32 TransferFamilyIndex;
} VulkanPhysicalDeviceQueueFamilyInfo;

Bool8 PhysicalDeviceMeetsRequirements(
    VkPhysicalDevice Device,
    VkSurfaceKHR Surface,
    const VkPhysicalDeviceProperties* Properties,
    const VkPhysicalDeviceFeatures* Features,
    const VulkanPhysicalDeviceRequirements* Requirements,
    VulkanPhysicalDeviceQueueFamilyInfo* QueueInfo,
    VulkanSwapchainSupportInfo* SwapchainSupport);

Int32 ScorePhysicalDevice(
    VkPhysicalDevice Device,
    const VkPhysicalDeviceProperties* Properties,
    const VkPhysicalDeviceFeatures* Features,
    const VulkanPhysicalDevicePreferences* Preferences
);

Bool8 SelectPhysicalDevice(VulkanContext* Context);

Bool8 VulkanDeviceCreate(VulkanContext* Context)
{
    if (!SelectPhysicalDevice(Context))
    {
        return false;
    }

    FLDEBUG("Creating logical device...");
    Bool8 PresentSharesGraphicsQueue = Context->Device.GraphicsQueueIndex == Context->Device.PresentQueueIndex;
    Bool8 ComputeSharesGraphicsQueue = Context->Device.GraphicsQueueIndex == Context->Device.ComputeQueueIndex;
    Bool8 TransferSharesGraphicsQueue = Context->Device.GraphicsQueueIndex == Context->Device.TransferQueueIndex;

    UInt32 IndexCount = 1;

    if (!PresentSharesGraphicsQueue)
    {
        IndexCount++;
    }

    if (!ComputeSharesGraphicsQueue)
    {
        IndexCount++;
    }

    if (!TransferSharesGraphicsQueue)
    {
        IndexCount++;
    }

    UInt32 Indices[IndexCount];
    UInt8 Index = 0;

    Indices[Index++] = Context->Device.GraphicsQueueIndex;

    if (!PresentSharesGraphicsQueue)
    {
        Indices[Index++] = Context->Device.PresentQueueIndex;
    }

    if (!ComputeSharesGraphicsQueue)
    {
        Indices[Index++] = Context->Device.ComputeQueueIndex;
    }

    if (!TransferSharesGraphicsQueue)
    {
        Indices[Index++] = Context->Device.TransferQueueIndex;
    }

    VkDeviceQueueCreateInfo QueueCreateInfos[IndexCount];

    for (UInt32 i = 0; i < IndexCount; i++)
    {
        Float32 QueuePriority = 1.0f;
        QueueCreateInfos[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        QueueCreateInfos[i].queueFamilyIndex = Indices[i];
        QueueCreateInfos[i].queueCount = 1;
        // Future Enhancement
        // if (Indices[i] == (UInt32)Context->Device.GraphicsQueueIndex)
        // {
        //     Float32 QueuePriority[2] = {1.0f, 1.0f};
        //     QueueCreateInfos[i].queueCount = 2;
        //     QueueCreateInfos[i].pQueuePriorities = &QueuePriority[i];
        // }
        QueueCreateInfos[i].flags = 0;
        QueueCreateInfos[i].pNext = 0;
        QueueCreateInfos[i].pQueuePriorities = &QueuePriority;
    }

    // Queries physical device capabilities
    VkPhysicalDeviceVulkan13Features Supported13 = { .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES };
    VkPhysicalDeviceVulkan12Features Supported12 = { .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES, .pNext = &Supported13 };
    VkPhysicalDeviceFeatures2 QueryFeatures = { .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, .pNext = &Supported12 };

    vkGetPhysicalDeviceFeatures2(Context->Device.PhysicalDevice, &QueryFeatures);

    // 2. Force-enable core 1.3 features
    VkPhysicalDeviceVulkan13Features Enable13 = {0};
    Enable13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
    Enable13.dynamicRendering = VK_TRUE; // Hard requirement
    Enable13.synchronization2 = VK_TRUE; // Hard requirement

    VkPhysicalDeviceVulkan12Features Enable12 = {0};
    Enable12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
    Enable12.pNext = &Enable13;
    Enable12.timelineSemaphore = (Context->Preferences.TimelineSemaphores && Supported12.timelineSemaphore) ? VK_TRUE : VK_FALSE;
    Enable12.descriptorIndexing = (Context->Preferences.DescriptorIndexing && Supported12.descriptorIndexing) ? VK_TRUE : VK_FALSE;
    Enable12.bufferDeviceAddress = (Context->Preferences.BufferDeviceAddress && Supported12.bufferDeviceAddress) ? VK_TRUE : VK_FALSE;

    VkPhysicalDeviceFeatures2 EnableFeatures2 = {0};
    EnableFeatures2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    EnableFeatures2.pNext = &Enable12;
    EnableFeatures2.features.samplerAnisotropy = QueryFeatures.features.samplerAnisotropy;
    EnableFeatures2.features.geometryShader    = (Context->Preferences.GeometryShader && QueryFeatures.features.geometryShader) ? VK_TRUE : VK_FALSE;
    EnableFeatures2.features.fillModeNonSolid  = (Context->Preferences.WireframeMode && QueryFeatures.features.fillModeNonSolid) ? VK_TRUE : VK_FALSE;

    // --- 3. Pass Feature Chain into VkDeviceCreateInfo ---
    VkDeviceCreateInfo DeviceCreateInfo = {0};
    DeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    DeviceCreateInfo.queueCreateInfoCount = IndexCount;
    DeviceCreateInfo.pQueueCreateInfos = QueueCreateInfos;
    
    // IMPORTANT: Set pEnabledFeatures to NULL when using VkPhysicalDeviceFeatures2 in pNext
    DeviceCreateInfo.pEnabledFeatures = NULL;
    DeviceCreateInfo.pNext = &EnableFeatures2;

    DeviceCreateInfo.enabledExtensionCount = 1;
    const char* Extensions = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
    DeviceCreateInfo.ppEnabledExtensionNames = &Extensions;

    // Deprecated and unused
    DeviceCreateInfo.enabledLayerCount = 0;
    DeviceCreateInfo.ppEnabledLayerNames = 0;

    VK_CHECK(vkCreateDevice(Context->Device.PhysicalDevice, &DeviceCreateInfo, Context->Allocator, &Context->Device.LogicalDevice));

    FLDEBUG("Logical device created");

    vkGetDeviceQueue(
        Context->Device.LogicalDevice,
        Context->Device.GraphicsQueueIndex,
        0,
        &Context->Device.GraphicsQueue);

    vkGetDeviceQueue(
        Context->Device.LogicalDevice,
        Context->Device.PresentQueueIndex,
        0,
        &Context->Device.PresentQueue);

    vkGetDeviceQueue(
        Context->Device.LogicalDevice,
        Context->Device.ComputeQueueIndex,
        0,
        &Context->Device.ComputeQueue);

    vkGetDeviceQueue(
        Context->Device.LogicalDevice,
        Context->Device.TransferQueueIndex,
        0,
        &Context->Device.TransferQueue);
    FLDEBUG("Queues Obtained");

    return true;
}

void VulkanDeviceDestroy(VulkanContext* Context)
{
    // Unset Queues
    Context->Device.GraphicsQueue = 0;
    Context->Device.PresentQueue = 0;
    Context->Device.ComputeQueue = 0;
    Context->Device.TransferQueue = 0;

    FLDEBUG("Destroying logical device...");
    if (Context->Device.LogicalDevice)
    {
        vkDestroyDevice(Context->Device.LogicalDevice, Context->Allocator);
        Context->Device.LogicalDevice = 0;
    }

    FLDEBUG("Releasing physical device resources...");
    Context->Device.PhysicalDevice = 0;

    if (Context->Device.SwapchainSupport.Formats)
    {
        FMFree(Context->Device.SwapchainSupport.Formats, sizeof(VkSurfaceFormatKHR) * Context->Device.SwapchainSupport.FormatCount, MEMORY_TAG_RENDERER);
        Context->Device.SwapchainSupport.Formats = 0;
        Context->Device.SwapchainSupport.FormatCount = 0;
    }

    if (Context->Device.SwapchainSupport.PresentModes)
    {
        FMFree(Context->Device.SwapchainSupport.PresentModes, sizeof(VkPresentModeKHR) * Context->Device.SwapchainSupport.PresentModeCount, MEMORY_TAG_RENDERER);
        Context->Device.SwapchainSupport.PresentModes = 0;
        Context->Device.SwapchainSupport.PresentModeCount = 0;
    }

    FMZeroMemory(&Context->Device.SwapchainSupport.Capabilities, sizeof(Context->Device.SwapchainSupport.Capabilities));

    Context->Device.GraphicsQueueIndex = (UInt32)-1;
    Context->Device.PresentQueueIndex =  (UInt32)-1;
    Context->Device.ComputeQueueIndex =  (UInt32)-1;
    Context->Device.TransferQueueIndex = (UInt32)-1;
}

void VulkanDeviceQuerySwapchainSupport(
    VkPhysicalDevice PhysicalDevice,
    VkSurfaceKHR Surface,
    VulkanSwapchainSupportInfo* SupportInfo)
{
    VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(PhysicalDevice, Surface, &SupportInfo->Capabilities));

    // Surface Formats
    VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(PhysicalDevice, Surface, &SupportInfo->FormatCount, 0));
    if (SupportInfo->FormatCount != 0)
    {
        if (!SupportInfo->Formats)
        {
            SupportInfo->Formats = FMAllocate(sizeof(VkSurfaceFormatKHR) * SupportInfo->FormatCount, MEMORY_TAG_RENDERER);
        }
        VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(PhysicalDevice, Surface, &SupportInfo->FormatCount, SupportInfo->Formats));
    }

    // Present Modes
    VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(PhysicalDevice, Surface, &SupportInfo->PresentModeCount, 0));
    if (SupportInfo->PresentModeCount != 0)
    {
        if (!SupportInfo->PresentModes)
        {
            SupportInfo->PresentModes = FMAllocate(sizeof(VkPresentModeKHR) * SupportInfo->PresentModeCount, MEMORY_TAG_RENDERER);
        }
        VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(PhysicalDevice, Surface, &SupportInfo->PresentModeCount, SupportInfo->PresentModes));
    }
}

Bool8 VulkanDeviceDetectDepthFormat(VulkanDevice* Device)
{
    const UInt8 CandidateCount = 5;
    VkFormat Candidates[3] = {
        VK_FORMAT_D32_SFLOAT,
        VK_FORMAT_D32_SFLOAT_S8_UINT,
        VK_FORMAT_D24_UNORM_S8_UINT
    };

    UInt32 Flags = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;

    for (UInt8 i = 0; i < CandidateCount; ++i) {
        VkFormatProperties Properties;
        vkGetPhysicalDeviceFormatProperties(Device->PhysicalDevice, Candidates[i], &Properties);

        // Attachment formats MUST support optimal tiling
        if ((Properties.optimalTilingFeatures & Flags) == Flags) {
            Device->DepthFormat = Candidates[i];
            return true;
        }
    }

    FLERROR("Failed to find a supported depth format!");
    return false;
}

Bool8 SelectPhysicalDevice(VulkanContext* Context)
{
    UInt32 PhysicalDeviceCount = 0;
    // VkPhysicalDevice PhysicalDevices[8];
    // UInt32 MaxDevices = 8;

    VK_CHECK(vkEnumeratePhysicalDevices(Context->Instance, &PhysicalDeviceCount, 0));

    if (PhysicalDeviceCount == 0)
    {
        FLFATAL("No devices which support Vulkan were found");
        return false;
    }

    // if (PhysicalDeviceCount > MaxDevices) {
    //     FLWARN("More than 8 GPUs found! Clamping to 8");
    //     PhysicalDeviceCount = MaxDevices;
    // }

    VkPhysicalDevice PhysicalDevices[PhysicalDeviceCount];
    VK_CHECK(vkEnumeratePhysicalDevices(Context->Instance, &PhysicalDeviceCount, PhysicalDevices));

    VkPhysicalDevice BestDevice = VK_NULL_HANDLE;
    Int32 BestScore = -1;
    VulkanPhysicalDeviceQueueFamilyInfo BestQueueInfo = {0};

    VkPhysicalDeviceProperties BestProperties = {0};
    VkPhysicalDeviceFeatures BestFeatures = {0};
    VkPhysicalDeviceMemoryProperties BestMemory = {0};

    VulkanPhysicalDevicePreferences BestPreferences = {0};

    Bool8 DeviceFound = false;

    // Hard Requirements (Must be met, or the engine crashes/exits)
    VulkanPhysicalDeviceRequirements Requirements = {0};
    Requirements.Graphics =          true;
    Requirements.Present =           true;
    Requirements.Compute =           true;
    Requirements.Transfer =          true;
    Requirements.SamplerAnisotropy = true;

    Requirements.DynamicRendering  = true;
    Requirements.Synchronization2  = true;

    // Initializes extension dynamic array
    Requirements.DeviceExtensionNames = VectorCreate(const char*);
    VectorAppend(Requirements.DeviceExtensionNames, (const char*)VK_KHR_SWAPCHAIN_EXTENSION_NAME);

    for (UInt32 i = 0; i < PhysicalDeviceCount; ++i)
    {
        VkPhysicalDeviceProperties Properties;
        vkGetPhysicalDeviceProperties(PhysicalDevices[i], &Properties);

        VkPhysicalDeviceFeatures Features;
        vkGetPhysicalDeviceFeatures(PhysicalDevices[i], &Features);

        VkPhysicalDeviceMemoryProperties Memory;
        vkGetPhysicalDeviceMemoryProperties(PhysicalDevices[i], &Memory);

        VulkanPhysicalDeviceQueueFamilyInfo QueueInfo = {0};

        Bool8 Result = PhysicalDeviceMeetsRequirements(
        PhysicalDevices[i],
        Context->Surface,
        &Properties,
        &Features,
        &Requirements,
        &QueueInfo,
        &Context->Device.SwapchainSupport);

        if (Result) 
        {
            DeviceFound = true;

            VulkanPhysicalDevicePreferences Preferences = {0};

            // --- DEVICE TYPE & MEMORY ---
            Preferences.DiscreteGPU                = true;
            Preferences.MinimumPreferredVRAM       = Gigabytes(4); // Baseline threshold

            // --- MODERN PIPELINE EXTENSIONS ---    
            Preferences.ShaderObjects              = true; 
            Preferences.GraphicsPipelineLibrary    = true; 
            Preferences.DescriptorIndexing         = true;

            // --- NEXT-GEN GEOMETRY & COMPUTE ---
            Preferences.RayTracing                 = true; 
            Preferences.MeshShaders                = true; 
            Preferences.CooperativeMatrix          = true; 

            // --- HARDWARE FEATURES ---
            Preferences.GeometryShader             = true;
            Preferences.WireframeMode              = true; 
            Preferences.TimelineSemaphores         = true; 
            Preferences.BufferDeviceAddress        = true; 

            // --- LIMITS ---
            Preferences.MinimumPushConstantsSize   = 256; 

            // Calculate scoring metrics for this pass
            Int32 CurrentScore = ScorePhysicalDevice(PhysicalDevices[i], &Properties, &Features, &Preferences);
            FLDEBUG("GPU [%s] calculated system score evaluation: %d", Properties.deviceName, CurrentScore);

            // Keep the card with the highest performance score
            if (CurrentScore > BestScore) 
            {
                BestScore = CurrentScore;
                BestDevice = PhysicalDevices[i];
                BestQueueInfo = QueueInfo;

                BestProperties = Properties;
                BestFeatures = Features;
                BestMemory = Memory;

                BestPreferences = Preferences;
            }
        }
        else 
        {
            FLWARN("GPU [%s] skipped: Failed hard engine requirements check.", Properties.deviceName);
        }
    }

    VectorDestroy(Requirements.DeviceExtensionNames);

    if (BestDevice == VK_NULL_HANDLE) 
    {
        FLFATAL("Zero detected system GPUs pass the mandatory hardware engine requirements.");
        return DeviceFound;
    }

    // Elects the winner into the context
    Context->Device.PhysicalDevice = BestDevice;
    Context->Device.GraphicsQueueIndex = BestQueueInfo.GraphicsFamilyIndex;
    Context->Device.PresentQueueIndex = BestQueueInfo.PresentFamilyIndex;
    Context->Device.ComputeQueueIndex = BestQueueInfo.ComputeFamilyIndex;
    Context->Device.TransferQueueIndex = BestQueueInfo.TransferFamilyIndex;

    // Cache the matching winning hardware statistics down into the Context
    Context->Device.Properties = BestProperties;
    Context->Device.Features = BestFeatures;
    Context->Device.Memory = BestMemory;

    // Cache the validated preferences for this winning device
    Context->Preferences = BestPreferences;

    vkGetPhysicalDeviceProperties(BestDevice, &BestProperties);
    // GPU type string mapping
    const char* DeviceTypeString = "Unknown";
    switch (BestProperties.deviceType) {
        case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
            DeviceTypeString = "Integrated";
            break;
        case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
            DeviceTypeString = "Discrete";
            break;
        case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
            DeviceTypeString = "Virtual";
            break;
        case VK_PHYSICAL_DEVICE_TYPE_CPU:
            DeviceTypeString = "CPU";
            break;
        default:
        case VK_PHYSICAL_DEVICE_TYPE_OTHER:
            DeviceTypeString = "Unknown";
            break;
    }

    // Combined clean final evaluation output
    FLINFO("--> Successfully elected physical system GPU: %s (Type: %s, Score: %d)", 
    BestProperties.deviceName, 
    DeviceTypeString, 
    BestScore);

    FLINFO(
          "GPU Driver version: %d.%d.%d",
          VK_VERSION_MAJOR(BestProperties.driverVersion),
          VK_VERSION_MINOR(BestProperties.driverVersion),
          VK_VERSION_PATCH(BestProperties.driverVersion));

    // Vulkan API version.
    FLINFO(
        "Vulkan API version: %d.%d.%d",
        VK_VERSION_MAJOR(BestProperties.apiVersion),
        VK_VERSION_MINOR(BestProperties.apiVersion),
        VK_VERSION_PATCH(BestProperties.apiVersion));
    
    // Memory information
        for (UInt32 j = 0; j < BestMemory.memoryHeapCount; ++j) {
         // Optimized: Cast to Float64 first to prevent 32-bit float precision truncation on large heaps
         Float64 MemorySizeGB = ((Float64)BestMemory.memoryHeaps[j].size) / 1024.0 / 1024.0 / 1024.0;
         
         if (BestMemory.memoryHeaps[j].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) {
             FLDEBUG("Local GPU memory: %.2f GB", MemorySizeGB);
         } else {
             FLDEBUG("Shared System memory: %.2f GB", MemorySizeGB);
         }
     }

    return DeviceFound;
}

Bool8 PhysicalDeviceMeetsRequirements(
    VkPhysicalDevice Device,
    VkSurfaceKHR Surface,
    const VkPhysicalDeviceProperties* Properties,
    const VkPhysicalDeviceFeatures* Features,
    const VulkanPhysicalDeviceRequirements* Requirements,
    VulkanPhysicalDeviceQueueFamilyInfo* QueueInfo,
    VulkanSwapchainSupportInfo* SwapchainSupport)
{
    (void)Features;
    QueueInfo->GraphicsFamilyIndex = (UInt32)-1;
    QueueInfo->PresentFamilyIndex = (UInt32)-1;
    QueueInfo->ComputeFamilyIndex = (UInt32)-1;
    QueueInfo->TransferFamilyIndex = (UInt32)-1;

    UInt32 QueueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(Device, &QueueFamilyCount, 0);
    VkQueueFamilyProperties* QueueFamilies = alloca(QueueFamilyCount * sizeof(VkQueueFamilyProperties));
    vkGetPhysicalDeviceQueueFamilyProperties(Device, &QueueFamilyCount, QueueFamilies);

    FLDEBUG("Evaluating Queue Families for: %s", Properties->deviceName);

    // Look at each queue and see what queues it supports
    FLDEBUG("              Graphics | Present | Compute | Transfer");
   UInt8 MinTransferScore = 255;

for (UInt32 i = 0; i < QueueFamilyCount; ++i) {
    VkBool32 SupportsPresent = VK_FALSE;
    VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(Device, i, Surface, &SupportsPresent));

    // Calculate queue pollution score
    UInt8 CurrentTransferScore = 0;
    if (QueueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) ++CurrentTransferScore;
    if (QueueFamilies[i].queueFlags & VK_QUEUE_COMPUTE_BIT)  ++CurrentTransferScore;
    if (QueueFamilies[i].queueFlags & VK_QUEUE_TRANSFER_BIT) ++CurrentTransferScore;
    if (SupportsPresent)                                     ++CurrentTransferScore;

    // 1. GRAPHICS QUEUE
    if (QueueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
        if (QueueInfo->GraphicsFamilyIndex == (UInt32)-1) {
            QueueInfo->GraphicsFamilyIndex = i;
        }
        
        // Prefer assigning Present to the same index as Graphics if supported
        if (SupportsPresent && QueueInfo->PresentFamilyIndex == (UInt32)-1) {
            QueueInfo->PresentFamilyIndex = i;
        }
    }

    // 2. PRESENT QUEUE (Fallback if Graphics queue family didn't support present)
    if (SupportsPresent && QueueInfo->PresentFamilyIndex == (UInt32)-1) {
        QueueInfo->PresentFamilyIndex = i;
    }

    // 3. ASYNC COMPUTE QUEUE
    if (QueueFamilies[i].queueFlags & VK_QUEUE_COMPUTE_BIT) {
        Bool8 HasGraphics = (QueueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0;
        Bool8 AlreadyFoundAsync = false;
        
        if (QueueInfo->ComputeFamilyIndex != (UInt32)-1) {
            AlreadyFoundAsync = (QueueFamilies[QueueInfo->ComputeFamilyIndex].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0;
        }

        // Prefer compute queues that do NOT have graphics capability (Async Compute)
        if (!HasGraphics && !AlreadyFoundAsync) {
            QueueInfo->ComputeFamilyIndex = i;
        } else if (QueueInfo->ComputeFamilyIndex == (UInt32)-1) {
            QueueInfo->ComputeFamilyIndex = i;
        }
    }

    // 4. DEDICATED TRANSFER QUEUE
    if (QueueFamilies[i].queueFlags & VK_QUEUE_TRANSFER_BIT) {
        if (CurrentTransferScore < MinTransferScore) {
            MinTransferScore = CurrentTransferScore;
            QueueInfo->TransferFamilyIndex = i;
        }
    }

    // Logging
    FLDEBUG("    Queue [%d]:   Graphics: %s | Present: %s | Compute: %s | Transfer: %s (Score: %d)", 
        i,
        (QueueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) ? "YES" : "NO ",
        SupportsPresent ? "YES" : "NO ",
        (QueueFamilies[i].queueFlags & VK_QUEUE_COMPUTE_BIT) ? "YES" : "NO ",
        (QueueFamilies[i].queueFlags & VK_QUEUE_TRANSFER_BIT) ? "YES" : "NO ",
        CurrentTransferScore
    );
}

    FLDEBUG("GPU [%s] Queue Selection Result -> Graphics: %s (%d) | Present: %s (%d) | Compute: %s (%d) | Transfer: %s (%d)",
        Properties->deviceName,
        (QueueInfo->GraphicsFamilyIndex != (UInt32)-1) ? "FOUND" : "MISSING", QueueInfo->GraphicsFamilyIndex,
        (QueueInfo->PresentFamilyIndex  != (UInt32)-1) ? "FOUND" : "MISSING", QueueInfo->PresentFamilyIndex,
        (QueueInfo->ComputeFamilyIndex  != (UInt32)-1) ? "FOUND" : "MISSING", QueueInfo->ComputeFamilyIndex,
        (QueueInfo->TransferFamilyIndex != (UInt32)-1) ? "FOUND" : "MISSING", QueueInfo->TransferFamilyIndex
    );

    // 4. Validate Requirements Verification Chain
    if ((!Requirements->Graphics || QueueInfo->GraphicsFamilyIndex != (UInt32)-1) &&
        (!Requirements->Present  || QueueInfo->PresentFamilyIndex  != (UInt32)-1) &&
        (!Requirements->Compute  || QueueInfo->ComputeFamilyIndex  != (UInt32)-1) &&
        (!Requirements->Transfer || QueueInfo->TransferFamilyIndex != (UInt32)-1)) 
    {
        FLDEBUG("Device [%s] meets queue requirements.", Properties->deviceName);
        FLTRACE("Graphics Family Index: %i", QueueInfo->GraphicsFamilyIndex);
        FLTRACE("Present Family Index:  %i", QueueInfo->PresentFamilyIndex);
        FLTRACE("Transfer Family Index: %i", QueueInfo->TransferFamilyIndex);
        FLTRACE("Compute Family Index:  %i", QueueInfo->ComputeFamilyIndex);

        VulkanDeviceQuerySwapchainSupport(Device, Surface, SwapchainSupport);

        if (SwapchainSupport->FormatCount < 1 || SwapchainSupport->PresentModeCount < 1)
        {
            if(SwapchainSupport->Formats) FMFree(SwapchainSupport->Formats, sizeof(VkSurfaceFormatKHR) *SwapchainSupport->FormatCount, MEMORY_TAG_RENDERER);
            if(SwapchainSupport->PresentModes) FMFree(SwapchainSupport->PresentModes, sizeof(VkPresentModeKHR) *SwapchainSupport->PresentModeCount, MEMORY_TAG_RENDERER);
            FLINFO("Required Swapchain support not present, skipping device");
            return false;
        }

        if (Requirements->DeviceExtensionNames)
        {
            UInt32 AvailableExtensionCount = 0;
            VkExtensionProperties* AvailableExtensions = 0;
            VK_CHECK(vkEnumerateDeviceExtensionProperties(Device, 0, &AvailableExtensionCount, 0));
            if (AvailableExtensionCount != 0)
            {
                AvailableExtensions = FMAllocate(sizeof(VkExtensionProperties) * AvailableExtensionCount, MEMORY_TAG_RENDERER);
                VK_CHECK(vkEnumerateDeviceExtensionProperties(Device, 0, &AvailableExtensionCount, AvailableExtensions));

                UInt32 RequiredExtensionCount = (UInt32)VectorSize(Requirements->DeviceExtensionNames);
                for (UInt32 i = 0; i < RequiredExtensionCount; ++i) {
                    Bool8 Found = false;
                    for (UInt32 j = 0; j < AvailableExtensionCount; ++j) {
                        if (StringCompare(Requirements->DeviceExtensionNames[i], AvailableExtensions[j].extensionName)) {
                            Found = true;
                            break;
                        }
                    }
                    if (!Found) {
                        FLWARN("Missing required extension: %s", Requirements->DeviceExtensionNames[i]);
                        FMFree(AvailableExtensions, sizeof(VkExtensionProperties) * AvailableExtensionCount, MEMORY_TAG_RENDERER);
                        return false;
                    }
                }
            }

            FMFree(AvailableExtensions, sizeof(VkExtensionProperties) * AvailableExtensionCount, MEMORY_TAG_RENDERER);
        }
        
        VkPhysicalDeviceVulkan13Features Req13Features = { .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES };
        VkPhysicalDeviceFeatures2 ReqFeatures2 = { .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, .pNext = &Req13Features };
        vkGetPhysicalDeviceFeatures2(Device, &ReqFeatures2);

        if (Requirements->DynamicRendering && !Req13Features.dynamicRendering) {
            FLWARN("Device [%s] dropped: Core Dynamic Rendering (Vulkan 1.3) unsupported.", Properties->deviceName);
            return false;
        }

        if (Requirements->Synchronization2 && !Req13Features.synchronization2) {
            FLWARN("Device [%s] dropped: Core Synchronization2 (Vulkan 1.3) unsupported.", Properties->deviceName);
            return false;
        }

        if (Requirements->SamplerAnisotropy && !Features->samplerAnisotropy) 
        {
            FLWARN("Device [%s] dropped: Device does not support SamplerAnisotrophy.", Properties->deviceName);
            return false;
        }

        return true; 
    } else {
        FLWARN("Device [%s] dropped: Missing required hardware to run.", Properties->deviceName);
        return false;
    }
    
}

Int32 ScorePhysicalDevice(
    VkPhysicalDevice Device,
    const VkPhysicalDeviceProperties* Properties,
    const VkPhysicalDeviceFeatures* Features,
    const VulkanPhysicalDevicePreferences* Preferences
)

{
    Int32 Score = 0;

    // 1. --- DEVICE TYPE & MEMORY ---
    if (Preferences->DiscreteGPU && Properties->deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
        Score += 1000; 
    } else if (Properties->deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) {
        Score += 100;  
    }

    VkPhysicalDeviceMemoryProperties MemoryProperties;
    vkGetPhysicalDeviceMemoryProperties(Device, &MemoryProperties);
    UInt64 TotalVRAM = 0;
    for (UInt32 i = 0; i < MemoryProperties.memoryHeapCount; ++i) {
        if (MemoryProperties.memoryHeaps[i].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) {
            TotalVRAM += MemoryProperties.memoryHeaps[i].size;
        }
    }
        if (TotalVRAM >= Preferences->MinimumPreferredVRAM) 
        {
        Score += 500;
        }
    
    UInt64 ExtraVRAMBytes = (TotalVRAM > Preferences->MinimumPreferredVRAM) ? (TotalVRAM - Preferences->MinimumPreferredVRAM) : 0;
    Float64 ExtraGigabytes = (Float64)ExtraVRAMBytes / (Float64)Gigabytes(1);
    
    // Diminishing returns: Give full points (100 per GB) up to 8GB extra (12GB total),
    // but only 25 points per GB for anything beyond that.
    if (ExtraGigabytes <= 8.0) {
        Score += (Int32)(ExtraGigabytes * 100.0);
    } else {
        // 8 GB extra gives a flat 800 points, then we scale down the rest
        Float64 ExcessiveVRAM = ExtraGigabytes - 8.0;
        Score += 800 + (Int32)(ExcessiveVRAM * 25.0);
    }

    // 2. --- EXTENSION FEATURE POINTER CHAIN QUERIES ---
    VkPhysicalDeviceVulkan11Features Features11 = {0};
    Features11.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;
    VkPhysicalDeviceVulkan12Features Features12 = {0};
    Features12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
    VkPhysicalDeviceVulkan13Features Features13 = {0};
    Features13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
    
    VkPhysicalDeviceShaderObjectFeaturesEXT ShaderObjectFeatures = {0};
    ShaderObjectFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_OBJECT_FEATURES_EXT;
    VkPhysicalDeviceGraphicsPipelineLibraryFeaturesEXT GPLFeatures = {0};
    GPLFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GRAPHICS_PIPELINE_LIBRARY_FEATURES_EXT;
    VkPhysicalDeviceMeshShaderFeaturesEXT MeshFeatures = {0};
    MeshFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_FEATURES_EXT;
    VkPhysicalDeviceAccelerationStructureFeaturesKHR ASFeatures = {0};
    ASFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR;
    VkPhysicalDeviceCooperativeMatrixFeaturesKHR CoopMatrixFeatures = {0};
    CoopMatrixFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COOPERATIVE_MATRIX_FEATURES_KHR;

    VkPhysicalDeviceFeatures2 ExtendedFeatures = {0};
    ExtendedFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    ExtendedFeatures.pNext = &Features11;
    Features11.pNext = &Features12;
    Features12.pNext = &Features13;
    Features13.pNext = &ShaderObjectFeatures;
    ShaderObjectFeatures.pNext = &GPLFeatures;
    GPLFeatures.pNext = &MeshFeatures;
    MeshFeatures.pNext = &ASFeatures;
    ASFeatures.pNext = &CoopMatrixFeatures;

    vkGetPhysicalDeviceFeatures2(Device, &ExtendedFeatures);

    // 3. --- EVALUATE EXTENSION PREFERENCES ---
    if (Preferences->DescriptorIndexing && Features12.descriptorIndexing)           Score += 100;
    if (Preferences->TimelineSemaphores && Features12.timelineSemaphore)            Score += 50;
    if (Preferences->BufferDeviceAddress && Features12.bufferDeviceAddress)          Score += 50;
    if (Preferences->ShaderObjects && ShaderObjectFeatures.shaderObject)            Score += 100;
    if (Preferences->GraphicsPipelineLibrary && GPLFeatures.graphicsPipelineLibrary) Score += 100;
    if (Preferences->MeshShaders && MeshFeatures.meshShader)                        Score += 200;
    if (Preferences->RayTracing && ASFeatures.accelerationStructure)                Score += 200;
    if (Preferences->CooperativeMatrix && CoopMatrixFeatures.cooperativeMatrix)     Score += 150;

    // 4. --- CORE 1.0 HARDWARE FEATURES ---
    if (Preferences->GeometryShader && Features->geometryShader)                    Score += 10;
    if (Preferences->WireframeMode && Features->fillModeNonSolid)                   Score += 50;

    // 5. --- HARDWARE LIMIT CONSTRAINTS ---
    if (Properties->limits.maxPushConstantsSize >= Preferences->MinimumPushConstantsSize) Score += 25;

    // 6. --- COMPUTE MUSCLE TIE-BREAKERS ---
    if (Properties->limits.maxImageDimension2D >= 16384)                            Score += 150;
    if (Properties->limits.maxBoundDescriptorSets >= 8)                             Score += 100;
    if (Properties->limits.maxComputeWorkGroupInvocations >= 1024)                  Score += 100;
    if (Properties->limits.maxPerStageDescriptorUniformBuffers >= 64)               Score += 50;
    if (Properties->limits.maxStorageBufferRange >= Gigabytes(4))                   Score += 100;
    if (Properties->limits.maxColorAttachments >= 8)                                Score += 25;
    if (Properties->limits.framebufferColorSampleCounts & VK_SAMPLE_COUNT_8_BIT)    Score += 50;
    if (Properties->limits.maxSamplerAnisotropy >= 16.0f)                           Score += 50;
    if (Properties->limits.maxComputeSharedMemorySize >= 65536)                     Score += 100;
    if (Properties->limits.maxVertexInputAttributes >= 32)                          Score += 25;

    return Score;
}