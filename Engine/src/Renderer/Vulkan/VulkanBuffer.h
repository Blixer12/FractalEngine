/**
 * @file VulkanBuffer.h
 * @author Blixer12
 * @brief A Vulkan-specific data buffer.
 * @version 1.0
 * @date 2026-10-11
 * 
 * @copyright Fractal Engine is Copyright (c) Blixer12 2026
 * 
 */

#pragma once

#include "VulkanDef.inl"

/**
 * @brief Creates a new Vulkan buffer.
 * 
 * @param Context A pointer to the Vulkan context.
 * @param Size The size of the buffer in bytes.
 * @param Usage The buffer usage flags (VkBufferUsageFlagBits)
 * @param MemoryPropertyFlags The memory property flags.
 * @param BindOnCreate Indicates if this buffer should bind on creation.
 * @param OutBuffer A pointer to hold the newly-created buffer.
 * @return True on success; otherwise false.
 */
Bool8 VulkanBufferCreate(
    VulkanContext* Context,
    UInt64 Size,
    VkBufferUsageFlagBits Usage,
    UInt32 MemoryPropertyFlags,
    Bool8 BindOnCreate,
    VulkanBuffer* OutBuffer);

/**
 * @brief Destroys the given buffer.
 * 
 * @param Context A pointer to the Vulkan context.
 * @param Buffer A pointer to the buffer to be destroyed.
 */
void VulkanBufferDestroy(VulkanContext* Context, VulkanBuffer* Buffer);

/**
 * @brief Resizes the given buffer. In this case, a new internal buffer of the given
 * size is created, data from the old buffer is copied to it, then the old buffer is
 * destroyed. This means this operation must be done when the buffer is not in use.
 * 
 * @param Context A pointer to the Vulkan context.
 * @param NewSize The new size of the buffer.
 * @param Buffer A pointer to the buffer to be resized.
 * @param Queue The queue used for the buffer resize operations.
 * @param Pool The command pool utilized for the internal temporary command buffer.
 * @return True on success; otherwise false.
 */
Bool8 VulkanBufferResize(
    VulkanContext* Context,
    UInt64 NewSize,
    VulkanBuffer* Buffer,
    VkQueue Queue,
    VkCommandPool Pool);

/**
 * @brief Binds the given buffer for use.
 * 
 * @param Context A pointer to the Vulkan context.
 * @param Buffer A pointer to the buffer to be bound.
 * @param Offset An offset in bytes to bind the buffer at.
 */
void VulkanBufferBind(VulkanContext* Context, VulkanBuffer* Buffer, UInt64 Offset);

/**
 * @brief Locks (or maps) the buffer memory to a temporary location of host memory, which should be unlocked before 
 * shutdown or destruction.
 * 
 * @param Context A pointer to the Vulkan context.
 * @param Buffer A pointer to the buffer whose memory should be locked.
 * @param Offset An offset in bytes to lock the memory at.
 * @param Size The amount of memory to lock.
 * @param Flags Flags to be used in the locking operation (VkMemoryMapFlags).
 * @return A pointer to a block of memory, mapped to the buffer's memory.
 */
void* VulkanBufferLockMemory(VulkanContext* Context, VulkanBuffer* Buffer, UInt64 Offset, UInt64 Size, UInt32 Flags);

/**
 * @brief Unlocks (or unmaps) the buffer memory.
 * 
 * @param Context A pointer to the Vulkan context.
 * @param Buffer A pointer to the buffer whose memory should be unlocked.
 */
void VulkanBufferUnlockMemory(VulkanContext* Context, VulkanBuffer* Buffer);

/**
 * @brief Allocates space from a vulkan buffer. Provides the offset at which the
 * allocation occurred. This will be required for data copying and freeing.
 * 
 * @param Buffer A pointer to the buffer from which to allocate.
 * @param Size The size in bytes to be allocated.
 * @param Offset A pointer to hold the offset in bytes from the beginning of the buffer.
 * @return True on success; otherwise false.
 */
Bool8 VulkanBufferAllocate(VulkanBuffer* Buffer, UInt64 Size, UInt64* Offset);

/**
 * @brief Frees space in the vulkan buffer.
 * 
 * @param Buffer A pointer to the buffer to free data from.
 * @param Size The size in bytes to be freed.
 * @param Offset The offset in bytes from the beginning of the buffer.
 * @return True on success; otherwise false.
 */
Bool8 VulkanBufferFree(VulkanBuffer* Buffer, UInt64 Size, UInt64 Offset);

/**
 * @brief Loads a data range into the given buffer at a given offset. Internally performs a map,
 * copy and unmap.
 * 
 * @param Context A pointer to the Vulkan context.
 * @param Buffer A pointer to the buffer to load into.
 * @param Offset The offset in bytes from the beginning of the buffer.
 * @param Size The amount of data in bytes that will be loaded.
 * @param Flags Flags to be used in the locking operation (VkMemoryMapFlags).
 * @param Data A pointer to the data to be loaded.
 */
void VulkanBufferLoadData(VulkanContext* Context, VulkanBuffer* Buffer, UInt64 Offset, UInt64 Size, UInt32 Flags, const void* Data);

/**
 * @brief Copies a range of data from one buffer to another.
 * 
 * @param Context A pointer to the Vulkan context.
 * @param Pool The command pool to be used.
 * @param Fence A fence to be used.
 * @param Queue The queue to be used.
 * @param Source The source buffer.
 * @param SourceOffset The source buffer offset.
 * @param Dest The destination buffer.
 * @param DestOffset The destination buffer offset.
 * @param Size The size of the data in bytes to be copied.
 */
void VulkanBufferCopyTo(
    VulkanContext* Context,
    VkCommandPool Pool,
    VkFence Fence,
    VkQueue Queue,
    VkBuffer Source,
    UInt64 SourceOffset,
    VkBuffer Dest,
    UInt64 DestOffset,
    UInt64 Size);