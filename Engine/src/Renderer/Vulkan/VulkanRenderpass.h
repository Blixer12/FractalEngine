/**
 * @file VulkanRenderpass.h
 * @author Blixer12
 * @brief This file contains the implementation of a Vulkan renderpass, which defines
 * attachments, potential clearing, and render area for the next drawing commands.
 * @version 1.0
 * @date 2026-10-11
 * 
 * @copyright Fractal Engine is Copyright (c) Blixer12 2026
 * 
 */

#pragma once

#include "VulkanDef.inl"

/**
 * @brief The types of clearing to be done on a renderpass.
 * Can be combined together for multiple clearing functions.
 */
typedef enum RenderpassClearFlag {
    /** @brief No clearing shoudl be done. */
    RENDERPASS_CLEAR_NONE_FLAG = 0x0,
     /** @brief Clear the colour buffer. */
    RENDERPASS_CLEAR_COLOR_BUFFER_FLAG = 0x1,
    /** @brief Clear the depth buffer. */
    RENDERPASS_CLEAR_DEPTH_BUFFER_FLAG = 0x2,
    /** @brief Clear the stencil buffer. */
    RENDERPASS_CLEAR_STENCIL_BUFFER_FLAG = 0x4,
} RenderpassClearFlag; 


/**
 * @brief Creates a new renderpass.
 * 
 * @param Context A pointer to the Vulkan context.
 * @param Renderpass A pointer to hold the newly-created renderpass.
 * @param RenderArea A rectangle to represent the render area. x and y are position, 
 * z is width and w is height.
 * @param ClearColor The color to be used when clearing the color buffer (RGBA, 0.0-1.0 range).
 * @param Depth The depth clear amount.
 * @param Stencil The stencil clear value.
 * @param ClearFlags The combined clear flags indicating what kind of clear should take place.
 * @param HasPreviousPass Indicates if there is a previous renderpass.
 * @param HasNextPass Indicates if there is a next renderpass.
 */
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


/**
 * @brief Destroys the given renderpass.
 * 
 * @param Context A pointer to the Vulkan context.
 * @param Renderpass A pointer to the renderpass to be destroyed.
 */
void VulkanRenderpassDestroy(VulkanContext* Context, VulkanRenderpass* Renderpass);


/**
 * @brief Begins the given renderpass.
 * 
 * @param CommandBuffer A pointer to the command buffer to be used.
 * @param Renderpass A pointer to the renderpass to begin.
 * @param Framebuffer The framebuffer to be used for the being of the renderpass.
 */
void VulkanRenderpassBegin(
    VulkanCommandBuffer* CommandBuffer, 
    VulkanRenderpass* Renderpass,
    VkFramebuffer Framebuffer);


/**
 * @brief Ends the given renderpass.
 * 
 * @param CommandBuffer A pointer to the command buffer to be used.
 * @param Renderpass A pointer to the renderpass to end.
 */
void VulkanRenderpassEnd(VulkanCommandBuffer* CommandBuffer, VulkanRenderpass* Renderpass);