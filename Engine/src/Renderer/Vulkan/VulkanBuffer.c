#include "VulkanBuffer.h"

#include "VulkanDevice.h"
#include "VulkanCommandBuffer.h"
#include "VulkanUtils.h"

#include "Core/Logger.h"
#include "Core/Memory.h"

Bool8 VulkanBufferCreate(
    VulkanContext* Context,
    UInt64 Size,
    VkBufferUsageFlagBits Usage,
    UInt32 MemoryPropertyFlags,
    Bool8 BindOnCreate,
    VulkanBuffer* Buffer)
    {
        FMZeroMemory(Buffer, sizeof(VulkanBuffer));
        Buffer->TotalSize = Size;
        Buffer->Usage = Usage;
        Buffer->MemoryPropertyFlags = MemoryPropertyFlags;

        VkBufferCreateInfo BufferInfo = {0};
        BufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        BufferInfo.size = Size;
        BufferInfo.usage = Usage;
        BufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VK_CHECK(vkCreateBuffer(Context->Device.LogicalDevice, &BufferInfo, Context->Allocator, &Buffer->Handle));

        // Gather memory Requirements.
        VkMemoryRequirements Requirements;
        vkGetBufferMemoryRequirements(Context->Device.LogicalDevice, Buffer->Handle, &Requirements);
        Buffer->MemoryIndex = Context->FindMemoryIndex(Requirements.memoryTypeBits, Buffer->MemoryPropertyFlags);
        if (Buffer->MemoryIndex == -1) {
            FLERROR("Unable to create vulkan Buffer because the required memory type index was not found.");
            return false;
        }

        // Allocate memory info
        VkMemoryAllocateInfo AllocateInfo = {0};
        AllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        AllocateInfo.allocationSize = Requirements.size;
        AllocateInfo.memoryTypeIndex = (UInt32)Buffer->MemoryIndex;

        // Allocate the memory.
        VkResult Result = vkAllocateMemory(
            Context->Device.LogicalDevice,
            &AllocateInfo,
            Context->Allocator,
            &Buffer->Memory);

        if (Result != VK_SUCCESS) {
            FLERROR("Unable to create vulkan Buffer because the required memory allocation failed. Error: %i", Result);
            return false;
        }

        if (BindOnCreate) {
            VulkanBufferBind(Context, Buffer, 0);
        }

        return true;
    }

void VulkanBufferDestroy(VulkanContext* Context, VulkanBuffer* Buffer)
{
    if (Buffer->Memory) {
        vkFreeMemory(Context->Device.LogicalDevice, Buffer->Memory, Context->Allocator);
        Buffer->Memory = 0;
    }
    if (Buffer->Handle) {
        vkDestroyBuffer(Context->Device.LogicalDevice, Buffer->Handle, Context->Allocator);
        Buffer->Handle = 0;
    }
    Buffer->TotalSize = 0;
    Buffer->Usage = 0;
    Buffer->IsLocked = false;
}

Bool8 VulkanBufferResize(
    VulkanContext* Context,
    UInt64 NewSize,
    VulkanBuffer* Buffer,
    VkQueue Queue,
    VkCommandPool Pool)
    {
        // Create new Buffer.
        VkBufferCreateInfo BufferInfo = {0};
        BufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        BufferInfo.size = NewSize;
        BufferInfo.usage = Buffer->Usage;
        BufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;  // NOTE: Only used in one queue.

        VkBuffer NewBuffer;
        VK_CHECK(vkCreateBuffer(Context->Device.LogicalDevice, &BufferInfo, Context->Allocator, &NewBuffer));

        // Gather memory Requirements.
        VkMemoryRequirements Requirements;
        vkGetBufferMemoryRequirements(Context->Device.LogicalDevice, NewBuffer, &Requirements);

        // Allocate memory info
        VkMemoryAllocateInfo AllocateInfo = {};
        AllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        AllocateInfo.allocationSize = Requirements.size;
        AllocateInfo.memoryTypeIndex = (UInt32)Buffer->MemoryIndex;

        // Allocate the memory.
        VkDeviceMemory NewMemory;
        VkResult Result = vkAllocateMemory(Context->Device.LogicalDevice, &AllocateInfo, Context->Allocator, &NewMemory);
        if (Result != VK_SUCCESS) {
            FLERROR("Unable to resize vulkan Buffer because the required memory allocation failed. Error: %i", Result);
            return false;
        }

        // Bind the new Buffer's memory
        VK_CHECK(vkBindBufferMemory(Context->Device.LogicalDevice, NewBuffer, NewMemory, 0));

        // Copy over the data
        VulkanBufferCopyTo(Context, Pool, 0, Queue, Buffer->Handle, 0, NewBuffer, 0, Buffer->TotalSize);

        // Make sure anything potentially using these is finished.
        vkDeviceWaitIdle(Context->Device.LogicalDevice);

        // Destroy the old
        if (Buffer->Memory) {
            vkFreeMemory(Context->Device.LogicalDevice, Buffer->Memory, Context->Allocator);
            Buffer->Memory = 0;
        }
        if (Buffer->Handle) {
            vkDestroyBuffer(Context->Device.LogicalDevice, Buffer->Handle, Context->Allocator);
            Buffer->Handle = 0;
        }

        // Set new properties
        Buffer->TotalSize = NewSize;
        Buffer->Memory = NewMemory;
        Buffer->Handle = NewBuffer;

        return true;
    }

void VulkanBufferBind(VulkanContext* Context, VulkanBuffer* Buffer, UInt64 Offset)
{
    VK_CHECK(vkBindBufferMemory(Context->Device.LogicalDevice, Buffer->Handle, Buffer->Memory, Offset));
}

void* VulkanBufferLockMemory(VulkanContext* Context, VulkanBuffer* Buffer, UInt64 Offset, UInt64 Size, UInt32 Flags)
{
    void* Data;
    VK_CHECK(vkMapMemory(Context->Device.LogicalDevice, Buffer->Memory, Offset, Size, Flags, &Data));
    return Data;
}
void VulkanBufferUnlockMemory(VulkanContext* Context, VulkanBuffer* Buffer)
{
    vkUnmapMemory(Context->Device.LogicalDevice, Buffer->Memory);
}

void VulkanBufferLoadData(VulkanContext* Context, VulkanBuffer* Buffer, UInt64 Offset, UInt64 Size, UInt32 Flags, const void* Data)
{
    void* DataPtr;
    VK_CHECK(vkMapMemory(Context->Device.LogicalDevice, Buffer->Memory, Offset, Size, Flags, &DataPtr));
    FMCopyMemory(DataPtr, Data, Size);
    vkUnmapMemory(Context->Device.LogicalDevice, Buffer->Memory);
}

void VulkanBufferCopyTo(
    VulkanContext* Context,
    VkCommandPool Pool,
    VkFence Fence,
    VkQueue Queue,
    VkBuffer Source,
    UInt64 SourceOffset,
    VkBuffer Dest,
    UInt64 DestOffset,
    UInt64 Size)
    {
        (void)Fence;
        
        vkQueueWaitIdle(Queue);
        // Create a one-time-use command buffer.
        VulkanCommandBuffer TemporaryCommandBuffer;
        VulkanCommandBufferAllocateAndBeginSingleUse(Context, Pool, &TemporaryCommandBuffer);

        // Prepare the copy command and add it to the command buffer.
        VkBufferCopy CopyRegion;
        CopyRegion.srcOffset = SourceOffset;
        CopyRegion.dstOffset = DestOffset;
        CopyRegion.size = Size;

        vkCmdCopyBuffer(TemporaryCommandBuffer.Handle, Source, Dest, 1, &CopyRegion);

        // Submit the buffer for execution and wait for it to complete.
        VulkanCommandBufferEndSingleUse(Context, Pool, &TemporaryCommandBuffer, Queue);
    }