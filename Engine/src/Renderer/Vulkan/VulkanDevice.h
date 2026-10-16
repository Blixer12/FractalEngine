/**
 * @file VulkanDevice.h
 * @author Blixer12
 * @brief This file contains a unified Vulkan device, which holds state and pointers
 * to both the Vulkan physical and logical devices, as well as other information such
 * as the swapchain.
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
 * @brief Creates a new Vulkan device and assigns it to the given context.
 * 
 * @param Context A pointer to the Vulkan context.
 * @return True on success; otherwise false.
 */
Bool8 VulkanDeviceCreate(VulkanContext* Context);

/**
 * @brief Destroys the device present in the given context.
 * 
 * @param Context A pointer to the Vulkan context.
 */
void VulkanDeviceDestroy(VulkanContext* Context);


/**
 * @brief Queries for swapchain support data for the given physical device and surface.
 * 
 * @param PhysicalDevice The Vulkan physical device.
 * @param Surface The Vulkan surface.
 * @param SupportInfo A pointer to hold the support info.
 */
void VulkanDeviceQuerySwapchainSupport(
    VkPhysicalDevice PhysicalDevice,
    VkSurfaceKHR Surface,
    VulkanSwapchainSupportInfo* SupportInfo);


/**
 * @brief Detects and assigns the depth format for the given device.
 * 
 * @param Device A pointer to the device.
 * @return True if successful; otherwise false.
 */
Bool8 VulkanDeviceDetectDepthFormat(VulkanDevice* Device);