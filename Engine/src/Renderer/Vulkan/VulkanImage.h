#pragma once

#include "VulkanDef.inl"

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
    VulkanImage* Image);

void VulkanImageViewCreate(
    VulkanContext* Context,
    VkFormat Format,
    VulkanImage* Image,
    VkImageAspectFlags AspectFlags);

/* 
 * Transitions the provided image from OldLayout to NewLayout
 */
void VulkanImageTransitionLayout(
    VulkanContext* Context,
    VulkanCommandBuffer* CommandBuffer,
    VulkanImage* Image,
    VkFormat Format,
    VkImageLayout OldLayout,
    VkImageLayout NewLayout);

/*
 * Copies data in the buffer to the provided image
 * @param Context The Culkan context
 * @param Image the image to copy the buffers data to
 * @param Buffer The buffer whose data will be copied
 */
void VulkanImageCopyFromBuffer(
    VulkanContext* Context,
    VulkanImage* Image,
    VkBuffer Buffer,
    VulkanCommandBuffer* CommandBuffer);

void VulkanImageDestroy(VulkanContext* Context, VulkanImage* Image);