#include "VulkanImage.h"

#include "VulkanDevice.h"

#include "Core/Memory.h"
#include "Core/Logger.h"

void VulkanImageCreate(
    VulkanContext* Context,
    VkImageType ImageType,
    UInt32 Width,
    UInt32 Height,
    VkFormat Format,
    VkImageTiling Tiling,
    VkImageUsageFlags Usage,
    VkMemoryPropertyFlags MemoryFlags,
    Bool32 CreateView,
    VkImageAspectFlags ViewAspectFlags,
    VulkanImage* Image)
    {
        (void)ImageType;
        Image->Width = Width;
        Image->Height = Height;

        // Creation info.
        VkImageCreateInfo ImageCreateInfo = {0};
        ImageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        ImageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
        ImageCreateInfo.extent.width = Width;
        ImageCreateInfo.extent.height = Height;
        ImageCreateInfo.extent.depth = 1;  // TODO: Support configurable depth.
        ImageCreateInfo.mipLevels = 4;     // TODO: Support mip mapping
        ImageCreateInfo.arrayLayers = 1;   // TODO: Support number of layers in the image.
        ImageCreateInfo.format = Format;
        ImageCreateInfo.tiling = Tiling;
        ImageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        ImageCreateInfo.usage = Usage;
        ImageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;          // TODO: Configurable sample count.
        ImageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;  // TODO: Configurable sharing mode.

        VK_CHECK(vkCreateImage(Context->Device.LogicalDevice, &ImageCreateInfo, Context->Allocator, &Image->Handle));

        VkMemoryRequirements MemoryRequirements;
        vkGetImageMemoryRequirements(Context->Device.LogicalDevice, Image->Handle, &MemoryRequirements);

        Int32 MemoryType = Context->FindMemoryIndex(MemoryRequirements.memoryTypeBits, MemoryFlags);
        if (MemoryType == -1)
        {
            FLERROR("Required Memory Types not found, Image is not Valid.");
        }

        VkMemoryAllocateInfo MemoryAllocateInfo = {0};
        MemoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        MemoryAllocateInfo.allocationSize = MemoryRequirements.size;
        MemoryAllocateInfo.memoryTypeIndex = MemoryType;
        VK_CHECK(vkAllocateMemory(Context->Device.LogicalDevice, &MemoryAllocateInfo, Context->Allocator, &Image->Memory));

        VK_CHECK(vkBindImageMemory(Context->Device.LogicalDevice, Image->Handle, Image->Memory, 0)); // TODO: Configurable memory offset

        if (CreateView)
        {
            Image->View = 0;
            VulkanImageViewCreate(Context, Format, Image, ViewAspectFlags);
        }
    }

void VulkanImageViewCreate(
    VulkanContext* Context,
    VkFormat Format,
    VulkanImage* Image,
    VkImageAspectFlags AspectFlags)
    {
        VkImageViewCreateInfo ViewCreateInfo = {0};
        ViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        ViewCreateInfo.image = Image->Handle;
        ViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;  // TODO: Make configurable.
        ViewCreateInfo.format = Format;
        ViewCreateInfo.subresourceRange.aspectMask = AspectFlags;

        // TODO: Make configurable
        ViewCreateInfo.subresourceRange.baseMipLevel = 0;
        ViewCreateInfo.subresourceRange.levelCount = 1;
        ViewCreateInfo.subresourceRange.baseArrayLayer = 0;
        ViewCreateInfo.subresourceRange.layerCount = 1;

        VK_CHECK(vkCreateImageView(Context->Device.LogicalDevice, &ViewCreateInfo, Context->Allocator, &Image->View));
    }

void VulkanImageDestroy(VulkanContext* Context, VulkanImage* Image)
{
    if (Image->View) {
        vkDestroyImageView(Context->Device.LogicalDevice, Image->View, Context->Allocator);
        Image->View = 0;
    }
    if (Image->Memory) {
        vkFreeMemory(Context->Device.LogicalDevice, Image->Memory, Context->Allocator);
        Image->Memory = 0;
    }
    if (Image->Handle) {
        vkDestroyImage(Context->Device.LogicalDevice, Image->Handle, Context->Allocator);
        Image->Handle = 0;
    }
}