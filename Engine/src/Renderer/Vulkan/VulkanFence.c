#include "VulkanFence.h"

#include "Core/Logger.h"

void VulkanFenceCreate(
    VulkanContext* Context,
    Bool8 CreateSignaled,
    VulkanFence* Fence) {
        
        Fence->IsSignaled = CreateSignaled;

        VkFenceCreateInfo FenceCreateInfo = {0};
        FenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        if (Fence->IsSignaled)
        {
            FenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
        }

        VK_CHECK(vkCreateFence(
            Context->Device.LogicalDevice,
            &FenceCreateInfo,
            Context->Allocator,
            &Fence->Handle));
    }

void VulkanFenceDestroy(VulkanContext* Context, VulkanFence* Fence)
{
    if (Fence->Handle)
    {
        vkDestroyFence(
            Context->Device.LogicalDevice,
            Fence->Handle,
            Context->Allocator);
        Fence->Handle = 0;
    }
    Fence->IsSignaled = false;
}

Bool8 VulkanFenceWait(VulkanContext* Context, VulkanFence* Fence, UInt64 TimeoutNanoseconds)
{
    if (!Fence->IsSignaled) {
        VkResult Result = vkWaitForFences(
            Context->Device.LogicalDevice,
            1,
            &Fence->Handle,
            true,
            TimeoutNanoseconds);

        switch (Result) {
            case VK_SUCCESS:
                Fence->IsSignaled = true;
                return true;
            case VK_TIMEOUT:
                FLWARN("VkFenceWait - Timed out");
                break;
            case VK_ERROR_DEVICE_LOST:
                FLERROR("VkFenceWait - VK_ERROR_DEVICE_LOST.");
                break;
            case VK_ERROR_OUT_OF_HOST_MEMORY:
                FLERROR("VkFenceWait - VK_ERROR_OUT_OF_HOST_MEMORY.");
                break;
            case VK_ERROR_OUT_OF_DEVICE_MEMORY:
                FLERROR("VkFenceWait - VK_ERROR_OUT_OF_DEVICE_MEMORY.");
                break;
            default:
                FLERROR("VkFenceWait - An unknown error has occurred.");
                break;
        }

    } else {
        return true;
    }

    return false;
}

void VulkanFenceReset(VulkanContext* Context, VulkanFence* Fence)
{
    if (Fence->IsSignaled)
    {
        VK_CHECK(vkResetFences(Context->Device.LogicalDevice, 1, &Fence->Handle));
        Fence->IsSignaled = false;
    }
}