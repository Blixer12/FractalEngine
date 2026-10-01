#pragma once

#include "VulkanDef.inl"

void VulkanFramebufferCreate(
    VulkanContext* Context,
    VulkanRenderpass* Renderpass,
    UInt32 Width,
    UInt32 Height,
    UInt32 AttachmentCount,
    VkImageView* Attachments,
    VulkanFramebuffer* Framebuffer);

void VulkanFramebufferDestroy(VulkanContext* Context, VulkanFramebuffer* Framebuffer);