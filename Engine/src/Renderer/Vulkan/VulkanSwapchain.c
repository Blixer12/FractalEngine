#include "VulkanSwapchain.h"

#include "Core/Logger.h"
#include "Core/Memory.h"
#include "VulkanImage.h"
#include "VulkanDevice.h"

// Helper function to create a swapchain
void Create(VulkanContext* Context, UInt32 Width, UInt32 Height, VulkanSwapchain* Swapchain);

// Helper function to destroy a swapchain
void Destroy(VulkanContext* Context, VulkanSwapchain* Swapchain);

void VulkanSwapchainCreate(
    VulkanContext* Context, 
    UInt32 Width, 
    UInt32 Height, 
    VulkanSwapchain* Swapchain)
    {
        Create(Context, Width, Height, Swapchain);
    }

void VulkanSwapchainRecreate(
    VulkanContext* Context, 
    UInt32 Width, 
    UInt32 Height, 
    VulkanSwapchain* Swapchain)
    {
        Destroy(Context, Swapchain);
        Create(Context, Width, Height, Swapchain);
    }

void VulkanSwapchainDestroy(
    VulkanContext* Context, 
    VulkanSwapchain* Swapchain)
    {
        Destroy(Context, Swapchain);
    }

Bool8 VulkanSwapchainAcquireNextImageIndex(
    VulkanContext* Context, 
    VulkanSwapchain* Swapchain, 
    UInt64 TimeoutNanoSeconds, 
    VkSemaphore ImageAvailableSemaphore, 
    VkFence Fence, 
    UInt32* ImageIndex)
    {
        VkResult Result = vkAcquireNextImageKHR(Context->Device.LogicalDevice, Swapchain->Handle, TimeoutNanoSeconds, ImageAvailableSemaphore, Fence, ImageIndex);

        if (Result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            VulkanSwapchainRecreate(Context, Context->FramebufferWidth, Context->FramebufferHeight, Swapchain);
            return false;

        } else if (Result != VK_SUCCESS && Result != VK_SUBOPTIMAL_KHR) {
            FLERROR("Could not acquire swapchain image (VkResult: %d)", Result);
            return false;
        }

        return true;
    }

void VulkanSwapchainPresent(
    VulkanContext* Context,
    VulkanSwapchain* Swapchain,
    VkQueue GraphicsQueue,
    VkQueue PresentQueue,
    VkSemaphore RenderCompleteSemaphore,
    UInt32 PresentImageIndex)
    {
        (void)GraphicsQueue;
        VkPresentInfoKHR PresentInfo = {0};
        PresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        PresentInfo.waitSemaphoreCount = 1;
        PresentInfo.pWaitSemaphores = &RenderCompleteSemaphore;
        PresentInfo.swapchainCount = 1;
        PresentInfo.pSwapchains = &Swapchain->Handle;
        PresentInfo.pImageIndices = &PresentImageIndex;
        PresentInfo.pResults = 0;

        VkResult Result = vkQueuePresentKHR(PresentQueue, &PresentInfo);

        if (Result == VK_ERROR_OUT_OF_DATE_KHR || Result == VK_SUBOPTIMAL_KHR)
        {
            VulkanSwapchainRecreate(Context, Context->FramebufferWidth, Context->FramebufferHeight, Swapchain);
            return;

        } else if (Result != VK_SUCCESS) {
            FLERROR("Failed to present swapchain image (VkResult: %d)", Result);
            return;
        }

    }

void Create(VulkanContext* Context, UInt32 Width, UInt32 Height, VulkanSwapchain* Swapchain)
{
    VkExtent2D SwapchainExtent = {Width, Height};
    Swapchain->MaxFramesInFlight = 2;

    VkSurfaceFormatKHR SwapchainFormat = {0};
    Bool8 FoundIdealFormat = false;

    Bool8 Support10Bit = false;
    Bool8 Support8BitBGRA_SRGB = false;
    Bool8 Support8BitRGBA_SRGB = false;
    Bool8 Support8BitBGRA_UNORM = false;
    Bool8 Support8BitRGBA_UNORM = false;

    VkSurfaceFormatKHR Format10Bit = {0};
    VkSurfaceFormatKHR Format8BitBGRA_SRGB = {0};
    VkSurfaceFormatKHR Format8BitRGBA_SRGB = {0};
    VkSurfaceFormatKHR Format8BitBGRA_UNORM = {0};
    VkSurfaceFormatKHR Format8BitRGBA_UNORM = {0};

    // Single-pass discovery loop
    for (UInt32 i = 0; i < Context->Device.SwapchainSupport.FormatCount; ++i) {
        VkSurfaceFormatKHR Format = Context->Device.SwapchainSupport.Formats[i];
        if (Format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            if (Format.format == VK_FORMAT_A2B10G10R10_UNORM_PACK32) {
                Support10Bit = true;
                Format10Bit = Format;
            } else if (Format.format == VK_FORMAT_B8G8R8A8_SRGB) {
                Support8BitBGRA_SRGB = true;
                Format8BitBGRA_SRGB = Format;
            } else if (Format.format == VK_FORMAT_R8G8B8A8_SRGB) {
                Support8BitRGBA_SRGB = true;
                Format8BitRGBA_SRGB = Format;
            } else if (Format.format == VK_FORMAT_B8G8R8A8_UNORM) {
                Support8BitBGRA_UNORM = true;
                Format8BitBGRA_UNORM = Format;
            } else if (Format.format == VK_FORMAT_R8G8B8A8_UNORM) {
                Support8BitRGBA_UNORM = true;
                Format8BitRGBA_UNORM = Format;
            }
        }
    }

    // Single-line priority evaluation (sRGB -> 10-bit -> UNORM fallback)
    if (Support8BitRGBA_SRGB) {
        SwapchainFormat = Format8BitRGBA_SRGB;
        FoundIdealFormat = true;
    } else if (Support8BitBGRA_SRGB) {
        SwapchainFormat = Format8BitBGRA_SRGB;
        FoundIdealFormat = true;
    } else if (Support10Bit) {
        SwapchainFormat = Format10Bit;
        FoundIdealFormat = true;
    } else if (Support8BitBGRA_UNORM) {
        SwapchainFormat = Format8BitBGRA_UNORM;
        FoundIdealFormat = true;
    } else if (Support8BitRGBA_UNORM) {
        SwapchainFormat = Format8BitRGBA_UNORM;
        FoundIdealFormat = true;
    }

    // Fallback
    if (!FoundIdealFormat) {
        SwapchainFormat = Context->Device.SwapchainSupport.Formats[0];
        FLWARN("Could not find an ideal surface format. Falling back to default driver format.");
    }

    Swapchain->ImageFormat = SwapchainFormat;

    // Set shader flag: UNORM formats require manual gamma encoding in shader
    if (SwapchainFormat.format == VK_FORMAT_A2B10G10R10_UNORM_PACK32 || 
        SwapchainFormat.format == VK_FORMAT_B8G8R8A8_UNORM ||
        SwapchainFormat.format == VK_FORMAT_R8G8B8A8_UNORM) { 
        Swapchain->IsFormatUnorm = true;
        } else {
            Swapchain->IsFormatUnorm = false;
        }

    VkPresentModeKHR PresentMode = VK_PRESENT_MODE_FIFO_KHR; // The absolute fallback guaranteed by Vulkan spec

    // Rank modes in order of preference
    Bool8 MailboxSupported = false;
    Bool8 FIFOLatestSupported = false;
    Bool8 FIFORelaxedSupported = false;
    Bool8 ImmediateSupported = false;

    for (UInt32 i = 0; i < Context->Device.SwapchainSupport.PresentModeCount; ++i) {
        VkPresentModeKHR Mode = Context->Device.SwapchainSupport.PresentModes[i];

        if (Mode == VK_PRESENT_MODE_MAILBOX_KHR) MailboxSupported = true;
        else if (Mode == VK_PRESENT_MODE_FIFO_LATEST_READY_KHR) FIFOLatestSupported = true;
        else if (Mode == VK_PRESENT_MODE_FIFO_RELAXED_KHR) FIFORelaxedSupported = true;
        else if (Mode == VK_PRESENT_MODE_IMMEDIATE_KHR) ImmediateSupported = true;
    }

    if (MailboxSupported) PresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
    else if (FIFOLatestSupported) PresentMode = VK_PRESENT_MODE_FIFO_LATEST_READY_KHR;
    else if (FIFORelaxedSupported) PresentMode = VK_PRESENT_MODE_FIFO_RELAXED_KHR;
    else if (ImmediateSupported) PresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;

    VulkanDeviceQuerySwapchainSupport(Context->Device.PhysicalDevice, Context->Surface, &Context->Device.SwapchainSupport);

    if (Context->Device.SwapchainSupport.Capabilities.currentExtent.width != UINT32_MAX)
    {
        SwapchainExtent =Context->Device.SwapchainSupport.Capabilities.currentExtent;
    }

    VkExtent2D Min = Context->Device.SwapchainSupport.Capabilities.minImageExtent;
    VkExtent2D Max = Context->Device.SwapchainSupport.Capabilities.maxImageExtent;
    SwapchainExtent.width = FCLAMP(SwapchainExtent.width, Min.width, Max.width);
    SwapchainExtent.height = FCLAMP(SwapchainExtent.height, Min.height, Max.height);

    UInt32 ImageCount = Context->Device.SwapchainSupport.Capabilities.minImageCount + 1;
    UInt32 MaxImageCount = Context->Device.SwapchainSupport.Capabilities.maxImageCount;

    if (MaxImageCount > 0 && ImageCount > MaxImageCount) {
        ImageCount = MaxImageCount;
    }

    VkSwapchainCreateInfoKHR SwapchainCreateInfo = {0};
    SwapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    SwapchainCreateInfo.surface = Context->Surface;
    SwapchainCreateInfo.minImageCount = ImageCount;
    SwapchainCreateInfo.imageFormat = Swapchain->ImageFormat.format;
    SwapchainCreateInfo.imageColorSpace = Swapchain->ImageFormat.colorSpace;
    SwapchainCreateInfo.imageExtent = SwapchainExtent;
    SwapchainCreateInfo.imageArrayLayers = 1;
    SwapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    SwapchainCreateInfo.presentMode = PresentMode;

    if (Context->Device.GraphicsQueueIndex != Context->Device.PresentQueueIndex)
    {
        UInt32 QueueFamilyIndices[] =
        {
            (UInt32)Context->Device.GraphicsQueueIndex,
            (UInt32)Context->Device.PresentQueueIndex
        };
        SwapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        SwapchainCreateInfo.queueFamilyIndexCount = 2;
        SwapchainCreateInfo.pQueueFamilyIndices = QueueFamilyIndices;
    } else {
        SwapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        SwapchainCreateInfo.queueFamilyIndexCount = 0;
        SwapchainCreateInfo.pQueueFamilyIndices = 0;
    }

    SwapchainCreateInfo.preTransform = Context->Device.SwapchainSupport.Capabilities.currentTransform;
    SwapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    SwapchainCreateInfo.clipped = VK_TRUE;
    SwapchainCreateInfo.oldSwapchain = 0;

    VK_CHECK(vkCreateSwapchainKHR(Context->Device.LogicalDevice, &SwapchainCreateInfo, Context->Allocator, &Swapchain->Handle));

    Context->CurrentFrame = 0;

    Swapchain->ImageCount = 0;

    VK_CHECK(vkGetSwapchainImagesKHR(Context->Device.LogicalDevice, Swapchain->Handle, &Swapchain->ImageCount, 0));
    if (!Swapchain->Images)
    {
        Swapchain->Images = (VkImage*)FMAllocate(sizeof(VkImage) * Swapchain->ImageCount, MEMORY_TAG_RENDERER);
    }

    if (!Swapchain->Views)
    {
        Swapchain->Views = (VkImageView*)FMAllocate(sizeof(VkImageView) * Swapchain->ImageCount, MEMORY_TAG_RENDERER);
    }
    VK_CHECK(vkGetSwapchainImagesKHR(Context->Device.LogicalDevice, Swapchain->Handle, &Swapchain->ImageCount, Swapchain->Images));

    for (UInt32 i = 0; i < Swapchain->ImageCount; i++)
    {
        VkImageViewCreateInfo ViewInfo = {0};
        ViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        ViewInfo.image = Swapchain->Images[i];
        ViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        ViewInfo.format = Swapchain->ImageFormat.format;
        ViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        ViewInfo.subresourceRange.baseMipLevel = 0;
        ViewInfo.subresourceRange.levelCount = 1;
        ViewInfo.subresourceRange.baseArrayLayer = 0;
        ViewInfo.subresourceRange.layerCount = 1;

        VK_CHECK(vkCreateImageView(Context->Device.LogicalDevice, &ViewInfo, Context->Allocator, &Swapchain->Views[i]));
    }

    if (!VulkanDeviceDetectDepthFormat(&Context->Device)) {
        Context->Device.DepthFormat = VK_FORMAT_UNDEFINED;
        FLERROR("Failed to detect supported depth format!");
        return;
    }

    FLDEBUG("Elected depth attachment format: %d", Context->Device.DepthFormat);

    VulkanImageCreate(
        Context,
        VK_IMAGE_TYPE_2D,
        SwapchainExtent.width,
        SwapchainExtent.height,
        Context->Device.DepthFormat,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        true,
        VK_IMAGE_ASPECT_DEPTH_BIT,
        &Swapchain->DepthAttachment);

    FLDEBUG("Swapchain created successfuly.");

}

void Destroy(VulkanContext* Context, VulkanSwapchain* Swapchain)
{
    VulkanImageDestroy(Context, &Swapchain->DepthAttachment);
    
    for (UInt32 i = 0; i < Swapchain->ImageCount; i++)
    {
        vkDestroyImageView(Context->Device.LogicalDevice, Swapchain->Views[i], Context->Allocator);
    }

    vkDestroySwapchainKHR(Context->Device.LogicalDevice, Swapchain->Handle, Context->Allocator);
}
