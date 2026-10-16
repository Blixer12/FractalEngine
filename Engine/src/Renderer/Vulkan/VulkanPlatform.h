/**
 * @file VulkanPlatform.h
 * @author Blixer12
 * @brief This file contains the "front end interface" for where the platform
 * and Vulkan meet. The implementation for this should exist in the corresponding
 * .c file for a given platform.
 * 
 * @version 1.1
 * @date 2026-10-11
 * 
 * @copyright Fractal Engine is Copyright (c) Blixer12 2026
 * 
 */

#pragma once

#include "Defines.h"

struct PlatformState;
struct VulkanContext;

/**
 * @brief Creates and assigns a surface to the given context.
 * 
 * @param Context A pointer to the Vulkan context.
 * @return True on success; otherwise false.
 */
Bool8 PlatformCreateVulkanSurface(struct VulkanContext* Context);

/**
 * @brief Appends the names of required extensions for this platform to
 * the ExtensionsVector, which should be created and passed in.
 * @param ExtensionsVector A pointer to the array names of required extension names. Must be a Vector
 * as this function adds names to the array.
 */
void PlatformGetRequiredExtensions(const char*** ExtensionsVector);