/**
 * @file VulkanSwapchain.h
 * @author Blixer12
 * @brief The Vulkan swapchain, which works with the framebuffer/attachments and
 * the surface to present an image to the screen.
 * @version 1.0
 * @date 2026-10-11
 * 
 * @copyright Fractal Engine is Copyright (c) Blixer12 2026
 * 
 */

#pragma once

#include "VulkanDef.inl"

/**
 * @brief Creates a new swapchain.
 * 
 * @param Context A pointer to the Vulkan context.
 * @param Width The initial width of the surface area.
 * @param Height The initial height of the surface area.
 * @param Swapchain A pointer to the newly-created swapchain.
 */
void VulkanSwapchainCreate(
    VulkanContext* Context, 
    UInt32 Width, 
    UInt32 Height, 
    VulkanSwapchain* Swapchain);


/**
 * @brief Recreates the given swapchain with the given width and
 * height, replacing the internal swapchain with the newly-created
 * one.
 * 
 * @param Context A pointer to the Vulkan context.
 * @param Width The new width of the surface area.
 * @param Height The new width of the surface area.
 * @param Swapchain A pointer to the swapchain to be recreated.
 */
void VulkanSwapchainRecreate(
    VulkanContext* Context, 
    UInt32 Width, 
    UInt32 Height, 
    VulkanSwapchain* Swapchain);


/**
 * @brief Destroys the given swapchain.
 * 
 * @param Context A pointer to the Vulkan context.
 * @param Swapchain A pointer to the swapchain to be destroyed.
 */
void VulkanSwapchainDestroy(
    VulkanContext* Context, 
    VulkanSwapchain* Swapchain);


/**
 * @brief Acquires the index of the next image to be rendered to.
 * 
 * @param Context A pointer to the Vulkan context.
 * @param Swapchain A pointer to the swapchain to acquire the image index from.
 * @param TimeoutNanoSeconds The maximum amount of time that can pass before the operation is considered failed.
 * @param ImageAvailableSemaphore A semaphore that will be signaled when this completes.
 * @param Fence A fence that will be signaled when this completes.
 * @param ImageIndex A pointer to hold the image index.
 * @return True on success; otherwise false.
 */
Bool8 VulkanSwapchainAcquireNextImageIndex(
    VulkanContext* Context, 
    VulkanSwapchain* Swapchain, 
    UInt64 TimeoutNanoSeconds, 
    VkSemaphore ImageAvailableSemaphore, 
    VkFence Fence, 
    UInt32* ImageIndex);


/**
 * @brief Presents the swapchain's current image to the surface.
 * 
 * @param Context A pointer to the Vulkan context.
 * @param Swapchain A pointer to the swapchain to present.
 * @param GraphicsQueue description The graphics queue used for presentation. 
 * TODO: remove because this is no longer needed. (Possibly)
 * @param PresentQueue The presentation queue used for presentation.
 * @param RenderCompleteSemaphore The semaphore that will be signaled when the presentation is complete.
 * @param PresentImageIndex The image index to present.
 */
void VulkanSwapchainPresent(
    VulkanContext* Context,
    VulkanSwapchain* Swapchain,
    VkQueue GraphicsQueue,
    VkQueue PresentQueue,
    VkSemaphore RenderCompleteSemaphore,
    UInt32 PresentImageIndex);