#pragma once

#include "VulkanDef.inl"

void VulkanFenceCreate(
    VulkanContext* Context,
    Bool8 CreateSignaled,
    VulkanFence* Fence);

void VulkanFenceDestroy(VulkanContext* Context, VulkanFence* Fence);

Bool8 VulkanFenceWait(VulkanContext* Context, VulkanFence* Fence, UInt64 TimeoutNanoseconds);

void VulkanFenceReset(VulkanContext* Context, VulkanFence* Fence);