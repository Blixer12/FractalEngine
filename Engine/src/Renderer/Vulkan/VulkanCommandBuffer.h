/**
 * @file VulkanCommandBuffer.h
 * @author Blixer12
 * @brief Represents a command buffer, which is used to hold commands to be
 * executed by a Vulkan queue.
 * 
 * @version 1.1
 * @date 2026-10-11
 * 
 * @copyright Fractal Engine is Copyright (c) Blixer12 2026
 * 
 */

#pragma once

#include "VulkanDef.inl"


/**
 * @brief Allocates a new command buffer from the given pool.
 * 
 * @param Context A pointer to the Vulkan context.
 * @param Pool The pool to allocate a command buffer from.
 * @param IsPrimary Indicates if the command buffer is a primary or secondary buffer.
 * @param CommandBuffer A pointer to hold the newly allocated command buffer.
 */
void VulkanCommandBufferAllocate(
    VulkanContext* Context,
    VkCommandPool Pool,
    Bool8 IsPrimary,
    VulkanCommandBuffer* CommandBuffer);


/**
 * @brief Frees the given command buffer and returns it to the provided pool.
 * 
 * @param Context A pointer to the Vulkan context.
 * @param Pool The pool to return the command buffer to.
 * @param CommandBuffer The command buffer to be returned.
 */
void VulkanCommandBufferFree(
    VulkanContext* Context,
    VkCommandPool Pool,
    VulkanCommandBuffer* CommandBuffer);


/**
 * @brief Begins the provided command buffer.
 * 
 * @param CommandBuffer A pointer to the command buffer to begin.
 * @param IsSingleUse Indicates if the buffer is just single use.
 * @param IsRenderpassContinue Indicates if the buffer is renderpass continue.
 * @param IsSimultaneousUse Indicates if the buffer is simultaneous use.
 */
void VulkanCommandBufferBegin(
    VulkanCommandBuffer* CommandBuffer,
    Bool8 IsSingleUse,
    Bool8 IsRenderpassContinue,
    Bool8 IsSimultaneousUse);


/**
 * @brief Ends the given command buffer.
 * 
 * @param CommandBuffer A pointer to the command buffer to end.
 */
void VulkanCommandBufferEnd(VulkanCommandBuffer* CommandBuffer);

/**
 * @brief Sets the command buffer to the submitted state.
 * 
 * @param CommandBuffer A pointer to the command buffer whose state to set.
 */
void VulkanCommandBufferUpdateSubmitted(VulkanCommandBuffer* CommandBuffer);

/**
 * @brief Resets the command buffer to the ready state.
 * 
 * @param CommandBuffer A pointer to the command buffer whose state should be set.
 */
void VulkanCommandBufferReset(VulkanCommandBuffer* CommandBuffer);

/**
 * @brief Allocates and begins recording to CommandBuffer
 * 
 * @param Context A pointer to the Vulkan context.
 * @param Pool The pool to obtain a command buffer from.
 * @param CommandBuffer A pointer to hold the allocated command buffer.
 */
void VulkanCommandBufferAllocateAndBeginSingleUse(
    VulkanContext* Context,
    VkCommandPool Pool,
    VulkanCommandBuffer* CommandBuffer);

/**
 * @brief Ends recording, submits to and waits for queue operation and frees the provided command buffer.
 * 
 * @param Context A pointer to the Vulkan context.
 * @param Pool The pool to return a command buffer to.
 * @param CommandBuffer A pointer to the command buffer to be returned.
 * @param Queue The queue to submit to.
 */
void VulkanCommandBufferEndSingleUse(
    VulkanContext* Context,
    VkCommandPool Pool,
    VulkanCommandBuffer* CommandBuffer,
    VkQueue Queue);