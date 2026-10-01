#pragma once

#include "VulkanDef.inl"

void VulkanCommandBufferAllocate(
    VulkanContext* Context,
    VkCommandPool Pool,
    Bool8 IsPrimary,
    VulkanCommandBuffer* CommandBuffer);

void VulkanCommandBufferFree(
    VulkanContext* Context,
    VkCommandPool Pool,
    VulkanCommandBuffer* CommandBuffer);

void VulkanCommandBufferBegin(
    VulkanCommandBuffer* CommandBuffer,
    Bool8 IsSingleUse,
    Bool8 IsRederpassContinue,
    Bool8 IsSimultaneousUse);

void VulkanCommandBufferEnd(VulkanCommandBuffer* CommandBuffer);

void VulkanCommandBufferUpdateSubmitted(VulkanCommandBuffer* CommandBuffer);

void VulkanCommandBufferReset(VulkanCommandBuffer* CommandBuffer);

/**
 * Allocates and begins recording to the Outputted ComandBuffer.
 */
void VulkanCommandBufferAllocateAndBeginSingleUse(
    VulkanContext* Context,
    VkCommandPool Pool,
    VulkanCommandBuffer* CommandBuffer);

/**
 * Ends recording, submits to and waits for queue operation and frees the provided command buffer.
 */
void VulkanCommandBufferEndSingleUse(
    VulkanContext* Context,
    VkCommandPool Pool,
    VulkanCommandBuffer* CommandBuffer,
    VkQueue Queue);