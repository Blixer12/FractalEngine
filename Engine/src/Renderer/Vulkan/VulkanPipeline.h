#pragma once

#include "VulkanDef.inl"

Bool8 VulkanGraphicsPipelineCreate(
    VulkanContext* Context,
    VulkanRenderpass* Renderpass,
    UInt32 AttributeCount,
    VkVertexInputAttributeDescription* Attributes,
    UInt32 DescriptorSetLayoutCount,
    VkDescriptorSetLayout* DescriptorSetLayouts,
    UInt32 StageCount,
    VkPipelineShaderStageCreateInfo* Stages,
    VkViewport Viewport,
    VkRect2D Scissor,
    Bool8 IsWireframe,
    VulkanPipeline* Pipeline
);

void VulkanPipelineDestroy(VulkanContext* Context, VulkanPipeline* Pipeline);

void VulkanPipelineBind(VulkanCommandBuffer* CommandBuffer, VkPipelineBindPoint BindPoint, VulkanPipeline Pipeline);