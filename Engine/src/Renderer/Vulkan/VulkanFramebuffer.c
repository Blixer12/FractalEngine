#include "VulkanFramebuffer.h"

#include "Core/Memory.h"

void VulkanFramebufferCreate(
    VulkanContext* Context,
    VulkanRenderpass* Renderpass,
    UInt32 Width,
    UInt32 Height,
    UInt32 AttachmentCount,
    VkImageView* Attachments,
    VulkanFramebuffer* Framebuffer) {

        Framebuffer->Attachments = FMAllocate(sizeof(VkImageView) * AttachmentCount, MEMORY_TAG_RENDERER);
        for (UInt32 i = 0; i < AttachmentCount; i++)
        {
            Framebuffer->Attachments[i] = Attachments[i];
        }

        Framebuffer->Renderpass = Renderpass;
        Framebuffer->AttachmentCount = AttachmentCount;

        VkFramebufferCreateInfo FramebufferCreateInfo = {0};
        FramebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        FramebufferCreateInfo.renderPass = Renderpass->Handle;
        FramebufferCreateInfo.attachmentCount = AttachmentCount;
        FramebufferCreateInfo.pAttachments = Framebuffer->Attachments;
        FramebufferCreateInfo.width = Width;
        FramebufferCreateInfo.height = Height;
        FramebufferCreateInfo.layers = 1;

        VK_CHECK(vkCreateFramebuffer(
            Context->Device.LogicalDevice,
            &FramebufferCreateInfo,
            Context->Allocator,
            &Framebuffer->Handle));
    }

void VulkanFramebufferDestroy(VulkanContext* Context, VulkanFramebuffer* Framebuffer)
{
    vkDestroyFramebuffer(Context->Device.LogicalDevice, Framebuffer->Handle, Context->Allocator);
    if (Framebuffer->Attachments) {
        FMFree(Framebuffer->Attachments, sizeof(VkImageView) * Framebuffer->AttachmentCount, MEMORY_TAG_RENDERER);
        Framebuffer->Attachments = 0;
    }
    Framebuffer->Handle = 0;
    Framebuffer->AttachmentCount = 0;
    Framebuffer->Renderpass = 0;
}