#include "VulkanCommandBuffer.h"

#include "Core/Memory.h"

void VulkanCommandBufferAllocate(
    VulkanContext* Context,
    VkCommandPool Pool,
    Bool8 IsPrimary,
    VulkanCommandBuffer* CommandBuffer) {

    FMZeroMemory(CommandBuffer, sizeof(CommandBuffer));

    VkCommandBufferAllocateInfo AllocateInfo = {0};
    AllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    AllocateInfo.commandPool = Pool;
    AllocateInfo.level = IsPrimary ? VK_COMMAND_BUFFER_LEVEL_PRIMARY : VK_COMMAND_BUFFER_LEVEL_SECONDARY;
    AllocateInfo.commandBufferCount = 1;
    AllocateInfo.pNext = 0;

    CommandBuffer->State = VULKAN_COMMAND_BUFFER_STATE_NOT_ALLOCATED;
    VK_CHECK(vkAllocateCommandBuffers(
        Context->Device.LogicalDevice,
        &AllocateInfo,
        &CommandBuffer->Handle));
    CommandBuffer->State = VULKAN_COMMAND_BUFFER_STATE_READY;
}

void VulkanCommandBufferFree(
    VulkanContext* Context,
    VkCommandPool Pool,
    VulkanCommandBuffer* CommandBuffer) {
    vkFreeCommandBuffers(
        Context->Device.LogicalDevice,
        Pool,
        1,
        &CommandBuffer->Handle);

    CommandBuffer->Handle = 0;
    CommandBuffer->State = VULKAN_COMMAND_BUFFER_STATE_NOT_ALLOCATED;
}

void VulkanCommandBufferBegin(
    VulkanCommandBuffer* CommandBuffer,
    Bool8 IsSingleUse,
    Bool8 IsRenderpassContinue,
    Bool8 IsSimultaneousUse) {
    
    VkCommandBufferBeginInfo BeginInfo = {0};
    BeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    BeginInfo.flags = 0;
    if (IsSingleUse) {
        BeginInfo.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    }
    if (IsRenderpassContinue) {
        BeginInfo.flags |= VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
    }
    if (IsSimultaneousUse) {
        BeginInfo.flags |= VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
    }

    VK_CHECK(vkBeginCommandBuffer(CommandBuffer->Handle, &BeginInfo));
    CommandBuffer->State = VULKAN_COMMAND_BUFFER_STATE_RECORDING;
}

void VulkanCommandBufferEnd(VulkanCommandBuffer* CommandBuffer) {
    VK_CHECK(vkEndCommandBuffer(CommandBuffer->Handle));
    CommandBuffer->State = VULKAN_COMMAND_BUFFER_STATE_RECORDING_ENDED;
}

void VulkanCommandBufferUpdateSubmitted(VulkanCommandBuffer* CommandBuffer) {
    CommandBuffer->State = VULKAN_COMMAND_BUFFER_STATE_SUBMITTED;
}

void VulkanCommandBufferReset(VulkanCommandBuffer* CommandBuffer) {
    CommandBuffer->State = VULKAN_COMMAND_BUFFER_STATE_READY;
}

void VulkanCommandBufferAllocateAndBeginSingleUse(
    VulkanContext* Context,
    VkCommandPool Pool,
    VulkanCommandBuffer* CommandBuffer) {
    VulkanCommandBufferAllocate(Context, Pool, true, CommandBuffer);
    VulkanCommandBufferBegin(CommandBuffer, true, false, false);
}

void VulkanCommandBufferEndSingleUse(
    VulkanContext* Context,
    VkCommandPool Pool,
    VulkanCommandBuffer* CommandBuffer,
    VkQueue Queue) {

    // End the command buffer.
    VulkanCommandBufferEnd(CommandBuffer);

    // Submit the queue
    VkSubmitInfo SubmitInfo = {0};
    SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    SubmitInfo.commandBufferCount = 1;
    SubmitInfo.pCommandBuffers = &CommandBuffer->Handle;
    VK_CHECK(vkQueueSubmit(Queue, 1, &SubmitInfo, 0));

    // Wait for it to finish
    VK_CHECK(vkQueueWaitIdle(Queue));

    // Free the command buffer.
    VulkanCommandBufferFree(Context, Pool, CommandBuffer);
 }