/**
 * @file VulkanImage.h
 * @author Blixer12
 * @brief The implementation of the Vulkan image, which can be thought of as a texture.
 * 
 * @version 1.0
 * @date 2026-10-11
 * 
 * @copyright Fractal Engine is Copyright (c) Blixer12 2026
 * 
 */

#pragma once

#include "VulkanDef.inl"

/**
 * @brief Creates a new Vulkan image.
 * 
 * @param Context A pointer to the Vulkan context.
 * @param ImageType The type of image.
 * @param Width The width of the image.
 * @param Height The height of the image.
 * @param Format The format of the image.
 * @param Tiling The image tiling mode.
 * @param Usage The image usage.
 * @param MemoryFlags Memory flags for the memory used by the image.
 * @param CreateView Indicates if a view should be created with the image.
 * @param ViewAspectFlags Aspect flags to be used when creating the view, if applicable.
 * @param Image A pointer to hold the newly-created image.
 */
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


/**
 * @brief Creates a view for the given image.
 * 
 * @param Context A pointer to the Vulkan context.
 * @param Format The image format.
 * @param Image A pointer to the image to associate the view with.
 * @param AspectFlags Aspect flags to be used when creating the view, if applicable.
 */
void VulkanImageViewCreate(
    VulkanContext* Context,
    VkFormat Format,
    VulkanImage* Image,
    VkImageAspectFlags AspectFlags);

/**
 * @brief Transitions the provided image from OldLayout to NewLayout.
 * 
 * @param Context A pointer to the Vulkan context.
 * @param CommandBuffer A pointer to the command buffer to be used.
 * @param Image A pointer to the image whose layout will be transitioned.
 * @param Format The image format.
 * @param OldLayout The old layout.
 * @param NewLayout The new layout.
 */
void VulkanImageTransitionLayout(
    VulkanContext* Context,
    VulkanCommandBuffer* CommandBuffer,
    VulkanImage* Image,
    VkFormat Format,
    VkImageLayout OldLayout,
    VkImageLayout NewLayout);

/**
 * Copies data in the buffer to the provided image
 * @param Context The Vulkan context
 * @param Image the image to copy the buffers data to
 * @param Buffer The buffer whose data will be copied
 * @param CommandBuffer A pointer to the command buffer to be used.
 */
void VulkanImageCopyFromBuffer(
    VulkanContext* Context,
    VulkanImage* Image,
    VkBuffer Buffer,
    VulkanCommandBuffer* CommandBuffer);


/**
 * @brief Destroys the given image.
 * 
 * @param Context A pointer to the Vulkan context.
 * @param Image A pointer to the image to be destroyed.
 */
void VulkanImageDestroy(VulkanContext* Context, VulkanImage* Image);