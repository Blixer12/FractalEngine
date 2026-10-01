#pragma once

#include "VulkanDef.inl"

void VulkanRenderpassCreate(
    VulkanContext* Context, 
    VulkanRenderpass* Renderpass,
    Float32 X, Float32 Y, Float32 W, Float32 H,
    Float32 R, Float32 G, Float32 B, Float32 A,
    Float32 Depth,
    UInt32 Stencil);

void VulkanRenderpassDestroy(VulkanContext* Context, VulkanRenderpass* Renderpass);

void VulkanRenderpassBegin(
    VulkanCommandBuffer* CommandBuffer, 
    VulkanRenderpass* Renderpass,
    VkFramebuffer Framebuffer);

void VulkanRenderpassEnd(VulkanCommandBuffer* CommandBuffer, VulkanRenderpass* Rendepass);