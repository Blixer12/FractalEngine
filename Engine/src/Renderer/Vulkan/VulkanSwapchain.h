#pragma once

#include "VulkanDef.inl"

void VulkanSwapchainCreate(
    VulkanContext* Context, 
    UInt32 Width, 
    UInt32 Height, 
    VulkanSwapchain* Swapchain);

void VulkanSwapchainRecreate(
    VulkanContext* Context, 
    UInt32 Width, 
    UInt32 Height, 
    VulkanSwapchain* Swapchain);

void VulkanSwapchainDestroy(
    VulkanContext* Context, 
    VulkanSwapchain* Swapchain);

Bool8 VulkanSwapchainAcquireNextImageIndex(
    VulkanContext* Context, 
    VulkanSwapchain* Swapchain, 
    UInt64 TimeoutNanoSeconds, 
    VkSemaphore ImageAvailableSemaphore, 
    VkFence Fence, 
    UInt32* ImageIndex);

void VulkanSwapchainPresent(
    VulkanContext* Context,
    VulkanSwapchain* Swapchain,
    VkQueue GraphicsQueue,
    VkQueue PresentQueue,
    VkSemaphore RenderCompleteSemaphore,
    UInt32 PresentImageIndex);