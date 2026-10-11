#include "VulkanPipeline.h"
#include "VulkanUtils.h"

#include "Core/Memory.h"
#include "Core/Logger.h"

#include "Math/MathDef.h"

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
    VulkanPipeline* Pipeline) {

    VkPipelineViewportStateCreateInfo ViewportState = {0};
    ViewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    ViewportState.viewportCount = 1;
    ViewportState.pViewports = &Viewport;
    ViewportState.scissorCount = 1;
    ViewportState.pScissors = &Scissor;

    VkPipelineRasterizationStateCreateInfo RasterizerCreateInfo = {0};
    RasterizerCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    RasterizerCreateInfo.depthClampEnable = VK_FALSE;
    RasterizerCreateInfo.polygonMode = IsWireframe ? VK_POLYGON_MODE_LINE : VK_POLYGON_MODE_FILL;
    RasterizerCreateInfo.lineWidth = 1.0f;
    RasterizerCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
    RasterizerCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    RasterizerCreateInfo.depthBiasEnable = VK_FALSE;
    RasterizerCreateInfo.depthBiasConstantFactor = 0.0f;
    RasterizerCreateInfo.depthBiasClamp = 0.0f;
    RasterizerCreateInfo.depthBiasSlopeFactor = 0.0f;

    VkPipelineMultisampleStateCreateInfo MultisamplingCreateInfo = {0};
    MultisamplingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    MultisamplingCreateInfo.sampleShadingEnable = VK_FALSE;
    MultisamplingCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    MultisamplingCreateInfo.minSampleShading = 1.0f;
    MultisamplingCreateInfo.pSampleMask = 0;
    MultisamplingCreateInfo.alphaToCoverageEnable = VK_FALSE;
    MultisamplingCreateInfo.alphaToOneEnable = VK_FALSE;

    VkPipelineDepthStencilStateCreateInfo DepthStencil = {0};
    if (DepthTestEnable)
    {
        DepthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        DepthStencil.depthTestEnable = VK_TRUE;
        DepthStencil.depthWriteEnable = VK_TRUE;
        DepthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
        DepthStencil.depthBoundsTestEnable = VK_FALSE;
        DepthStencil.stencilTestEnable = VK_FALSE;
    }

    VkPipelineColorBlendAttachmentState ColorBlendAttachmentState;
    FMZeroMemory(&ColorBlendAttachmentState, sizeof(VkPipelineColorBlendAttachmentState));
    ColorBlendAttachmentState.blendEnable = VK_TRUE;
    ColorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    ColorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    ColorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
    ColorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    ColorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    ColorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;
    ColorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                               VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    
    VkPipelineColorBlendStateCreateInfo ColorBlendStateInfo = {0};
    ColorBlendStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    ColorBlendStateInfo.logicOpEnable = VK_FALSE;
    ColorBlendStateInfo.logicOp = VK_LOGIC_OP_COPY;
    ColorBlendStateInfo.attachmentCount = 1;
    ColorBlendStateInfo.pAttachments = &ColorBlendAttachmentState;

    constexpr UInt32 DynamicStateCount = 3;
    VkDynamicState DynamicStates[DynamicStateCount] = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR,
        VK_DYNAMIC_STATE_LINE_WIDTH};

    VkPipelineDynamicStateCreateInfo DynamicStateCreateInfo = {0};
    DynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    DynamicStateCreateInfo.dynamicStateCount = DynamicStateCount;
    DynamicStateCreateInfo.pDynamicStates = DynamicStates;

    VkVertexInputBindingDescription BindingDescription;
    BindingDescription.binding = 0;
    BindingDescription.stride = Stride;
    BindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    VkPipelineVertexInputStateCreateInfo VertexInputInfo = {0};
    VertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    VertexInputInfo.vertexBindingDescriptionCount = 1;
    VertexInputInfo.pVertexBindingDescriptions = &BindingDescription;
    VertexInputInfo.vertexAttributeDescriptionCount = AttributeCount;
    VertexInputInfo.pVertexAttributeDescriptions = Attributes;

    VkPipelineInputAssemblyStateCreateInfo InputAssemblyCreateInfo = {0};
    InputAssemblyCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    InputAssemblyCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    InputAssemblyCreateInfo.primitiveRestartEnable = VK_FALSE;

    VkPipelineLayoutCreateInfo PipelineLayoutCreateInfo = {0};
    PipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    
    // Push Constants
    VkPushConstantRange PushConstant;
    PushConstant.stageFlags = VK_SHADER_STAGE_VERTEX_BIT; //| VK_SHADER_STAGE_FRAGMENT_BIT;
    PushConstant.offset = sizeof(Mat4) * 0;
    PushConstant.size = sizeof(Mat4) * 2;
    PipelineLayoutCreateInfo.pushConstantRangeCount = 1;
    PipelineLayoutCreateInfo.pPushConstantRanges = &PushConstant;

    // Descriptor set layouts
    PipelineLayoutCreateInfo.setLayoutCount = DescriptorSetLayoutCount;
    PipelineLayoutCreateInfo.pSetLayouts = DescriptorSetLayouts;

    VK_CHECK(vkCreatePipelineLayout(
        Context->Device.LogicalDevice,
        &PipelineLayoutCreateInfo,
        Context->Allocator,
        &Pipeline->PipelineLayout));

    VkGraphicsPipelineCreateInfo PipelineCreateInfo = {0};
    PipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    PipelineCreateInfo.stageCount = StageCount;
    PipelineCreateInfo.pStages = Stages;
    PipelineCreateInfo.pInputAssemblyState = &InputAssemblyCreateInfo;
    PipelineCreateInfo.pVertexInputState = &VertexInputInfo;

    PipelineCreateInfo.pViewportState = &ViewportState;
    PipelineCreateInfo.pRasterizationState = &RasterizerCreateInfo;
    PipelineCreateInfo.pMultisampleState = &MultisamplingCreateInfo;
    PipelineCreateInfo.pDepthStencilState = DepthTestEnable ? &DepthStencil : 0;
    PipelineCreateInfo.pColorBlendState = &ColorBlendStateInfo;
    PipelineCreateInfo.pDynamicState = &DynamicStateCreateInfo;
    PipelineCreateInfo.pTessellationState = 0;

    PipelineCreateInfo.layout = Pipeline->PipelineLayout;

    PipelineCreateInfo.renderPass = Renderpass->Handle;
    PipelineCreateInfo.subpass = 0;
    PipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
    PipelineCreateInfo.basePipelineIndex = (UInt32)-1;

    VkResult Result = vkCreateGraphicsPipelines(
        Context->Device.LogicalDevice,
        VK_NULL_HANDLE,
        1,
        &PipelineCreateInfo,
        Context->Allocator,
        &Pipeline->Handle);
    if (VulkanResultIsSuccess(Result))
    {
        FLDEBUG("Graphics pipeline created");
        return true;
    }

    FLERROR("vkCreateGraphicsPipelines failed with %s.", VulkanResultString(Result, true));
    return false;
}

void VulkanPipelineDestroy(VulkanContext* Context, VulkanPipeline* Pipeline)
{
    if (Pipeline)
    {
        if (Pipeline->Handle)
        {
            vkDestroyPipeline(Context->Device.LogicalDevice, Pipeline->Handle, Context->Allocator);
            Pipeline->Handle = 0;
        }

        if (Pipeline->PipelineLayout)
        {
            vkDestroyPipelineLayout(Context->Device.LogicalDevice, Pipeline->PipelineLayout, Context->Allocator);
            Pipeline->PipelineLayout = 0;
        }
    }
}

void VulkanPipelineBind(VulkanCommandBuffer* CommandBuffer, VkPipelineBindPoint BindPoint, VulkanPipeline Pipeline)
{
    vkCmdBindPipeline(CommandBuffer->Handle, BindPoint, Pipeline.Handle);
}