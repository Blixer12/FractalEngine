/**
 * @file VulkanPipeline.h
 * @author Blixer12
 * @brief This file contains a Vulkan pipeline, which is responsible for combining
 * items such as the shader modules, attributes, uniforms/descriptors, viewport/scissor,
 * 
 * @version 1.1
 * @date 2026-10-11
 * 
 * @copyright Fractal Engine is Copyright (c) Blixer12 2026
 * 
 */

#pragma once

#include "VulkanDef.inl"


/**
 * @brief Creates a new Vulkan pipeline.
 * 
 * @param Context A pointer to the Vulkan context.
 * @param Renderpass A pointer to the renderpass to associate with the pipeline.
 * @param Stride The stride of the vertex data to be used (ex: sizeof(vertex_3d))
 * @param AttributeCount The number of attributes.
 * @param Attributes An array of attributes.
 * @param DescriptorSetLayoutCount The number of descriptor set layouts.
 * @param DescriptorSetLayouts An array of descriptor set layouts.
 * @param StageCount The number of stages (vertex, fragment, etc).
 * @param Stages An array of stages.
 * @param Viewport The viewport configuration.
 * @param Scissor The scissor configuration.
 * @param IsWireframe Indicates if this pipeline should use wireframe mode.
 * @param DepthTestEnable Indicates if depth testing is enabled for this pipeline/
 * @param Pipeline A pointer to hold the newly-created pipeline.
 * @return True on success; otherwise false.
 */
Bool8 VulkanGraphicsPipelineCreate(
    VulkanContext* Context,
    VulkanRenderpass* Renderpass,
    UInt32 Stride,
    UInt32 AttributeCount,
    VkVertexInputAttributeDescription* Attributes,
    UInt32 DescriptorSetLayoutCount,
    VkDescriptorSetLayout* DescriptorSetLayouts,
    UInt32 StageCount,
    VkPipelineShaderStageCreateInfo* Stages,
    VkViewport Viewport,
    VkRect2D Scissor,
    Bool8 IsWireframe,
    Bool8 DepthTestEnable,
    VulkanPipeline* Pipeline);


/**
 * @brief Destroys the given pipeline.
 * 
 * @param Context A pointer to the Vulkan context.
 * @param Pipeline A pointer to the pipeline to be destroyed.
 */
void VulkanPipelineDestroy(VulkanContext* Context, VulkanPipeline* Pipeline);


/**
 * @brief Binds the given pipeline for use. This must be done within a renderpass.
 * 
 * @param CommandBuffer The command buffer to assign the bind command to.
 * @param BindPoint The pipeline bind point (typically bind_point_graphics)
 * @param Pipeline A pointer to the pipeline to be bound.
 */
void VulkanPipelineBind(VulkanCommandBuffer* CommandBuffer, VkPipelineBindPoint BindPoint, VulkanPipeline Pipeline);