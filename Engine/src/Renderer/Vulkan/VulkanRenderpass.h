#pragma once

#include "VulkanDef.inl"

typedef enum RenderpassClearFlag {
    RENDERPASS_CLEAR_NONE_FLAG = 0x0,
    RENDERPASS_CLEAR_COLOR_BUFFER_FLAG = 0x1,
    RENDERPASS_CLEAR_DEPTH_BUFFER_FLAG = 0x2,
    RENDERPASS_CLEAR_STENCIL_BUFFER_FLAG = 0x4,
} RenderpassClearFlag; 

void VulkanRenderpassCreate(
    VulkanContext* Context, 
    VulkanRenderpass* Renderpass,
    Vec4 RenderArea,
    Vec4 ClearColor,
    Float32 Depth,
    UInt32 Stencil,
    UInt8 ClearFlags,
    Bool8 HasPreviousPass,
    Bool8 HasNextPass);

void VulkanRenderpassDestroy(VulkanContext* Context, VulkanRenderpass* Renderpass);

void VulkanRenderpassBegin(
    VulkanCommandBuffer* CommandBuffer, 
    VulkanRenderpass* Renderpass,
    VkFramebuffer Framebuffer);

void VulkanRenderpassEnd(VulkanCommandBuffer* CommandBuffer, VulkanRenderpass* Rendepass);