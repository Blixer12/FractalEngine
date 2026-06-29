#pragma once

#include "Defines.h"

#include <vulkan/vulkan.h>

typedef struct VulkanContext {
    VkInstance Instance;
    VkAllocationCallbacks* Allocator;
} VulkanContext;