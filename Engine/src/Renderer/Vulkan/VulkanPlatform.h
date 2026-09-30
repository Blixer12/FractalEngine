#pragma once

#include "Defines.h"

struct PlatformState;
struct VulkanContext;

Bool8 PlatformCreateVulkanSurface(struct PlatformState* Platform, struct VulkanContext* Context);

/**
 * Appends the names of required extensions for this platform to
 * the ExtensionsVector, which should be created and passed in.
 */
void PlatformGetRequiredExtensions(const char*** ExtensionsVector);