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

void VulkanImageTransitionLayout(
    VulkanContext* Context,
    VulkanCommandBuffer* CommandBuffer,
    VulkanImage* Image,
    VkFormat Format,
    VkImageLayout OldLayout,
    VkImageLayout NewLayout) {

        (void)Format;

        VkImageMemoryBarrier Barrier = {0};
        Barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        Barrier.oldLayout = OldLayout;
        Barrier.newLayout = NewLayout;
        Barrier.srcQueueFamilyIndex = Context->Device.GraphicsQueueIndex;
        Barrier.dstQueueFamilyIndex = Context->Device.GraphicsQueueIndex;
        Barrier.image = Image->Handle;
        Barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        Barrier.subresourceRange.baseMipLevel = 0;
        Barrier.subresourceRange.levelCount = 1;
        Barrier.subresourceRange.baseArrayLayer = 0;
        Barrier.subresourceRange.layerCount = 1;

        VkPipelineStageFlags SourceStage;
        VkPipelineStageFlags DestinationStage;

        // we dont care about the old layout - transition to optimal layout
        if (OldLayout == VK_IMAGE_LAYOUT_UNDEFINED && NewLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
        {
            Barrier.srcAccessMask = 0;
            Barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            // we dont care what stage it is in
            SourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

            // for Copying
            DestinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        } else if (OldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && NewLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
        {
            Barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            Barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            SourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;

            DestinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        } else {
            FLERROR("Unsupported Image Layout Transition");
            return;
        }

        vkCmdPipelineBarrier(
            CommandBuffer->Handle,
            SourceStage, DestinationStage,
            0,
            0, 0,
            0, 0,
            1, &Barrier);

    }

void VulkanImageCopyFromBuffer(
    VulkanContext* Context,
    VulkanImage* Image,
    VkBuffer Buffer,
    VulkanCommandBuffer* CommandBuffer) {

        (void)Context;

        VkBufferImageCopy Region;
        FMZeroMemory(&Region, sizeof(Region));
        Region.bufferOffset = 0;
        Region.bufferRowLength = 0;
        Region.bufferImageHeight = 0;

        
        Region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        Region.imageSubresource.mipLevel = 0;
        Region.imageSubresource.baseArrayLayer = 0;
        Region.imageSubresource.layerCount = 1;

        Region.imageExtent.width = Image->Width;
        Region.imageExtent.height = Image->Height;
        Region.imageExtent.depth = 1;

        vkCmdCopyBufferToImage(
            CommandBuffer->Handle,
            Buffer,
            Image->Handle,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1,
            &Region);

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