#pragma once

#include "VulkanDef.inl"

Bool8 VulkanDeviceCreate(VulkanContext* Context);
void VulkanDeviceDestroy(VulkanContext* Context);

void VulkanDeviceQuerySwapchainSupport(
    VkPhysicalDevice PhysicalDevice,
    VkSurfaceKHR Surface,
    VulkanSwapchainSupportInfo* SupportInfo);

Bool8 VulkanDeviceDetectDepthFormat(VulkanDevice* Device);