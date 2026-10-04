#pragma once

#include "VulkanDef.inl"

Bool8 VulkanBufferCreate(
    VulkanContext* Context,
    UInt64 Size,
    VkBufferUsageFlagBits Usage,
    UInt32 MemoryPropertyFlags,
    Bool8 BindOnCreate,
    VulkanBuffer* Buffer);

void VulkanBufferDestroy(VulkanContext* Context, VulkanBuffer* Buffer);

Bool8 VulkanBufferResize(
    VulkanContext* Context,
    UInt64 NewSize,
    VulkanBuffer* Buffer,
    VkQueue Queue,
    VkCommandPool Pool);

void VulkanBufferBind(VulkanContext* Context, VulkanBuffer* Buffer, UInt64 Offset);

void* VulkanBufferLockMemory(VulkanContext* Context, VulkanBuffer* Buffer, UInt64 Offset, UInt64 Size, UInt32 Flags);
void VulkanBufferUnlockMemory(VulkanContext* Context, VulkanBuffer* Buffer);

void VulkanBufferLoadData(VulkanContext* Context, VulkanBuffer* Buffer, UInt64 Offset, UInt64 Size, UInt32 Flags, const void* Data);

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