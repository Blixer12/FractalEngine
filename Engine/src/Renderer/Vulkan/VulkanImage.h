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

void VulkanImageDestroy(VulkanContext* Context, VulkanImage* Image);

void VulkanImageTransitionLayout(
    VkCommandBuffer CommandBuffer,
    VkImage Image,
    VkImageLayout OldLayout,
    VkImageLayout NewLayout,
    VkImageAspectFlags AspectMask);