// Standard Library
#include <fstream>
#include <stdexcept>
#include <iostream>
#include <cassert>

#include "Pipeline.h"
#include "Model.h"

namespace FractalEngine
{
    FractalPipeline::FractalPipeline(
        FractalDevice &Device,
        const PipelineConfigInfo &ConfigInfo,
        const std::string &VertFilePath,
        const std::string &FragFilePath) : fractalDevice{Device}
    {
        CreateGraphicsPipeline(ConfigInfo, VertFilePath, FragFilePath);
    }

    FractalPipeline::~FractalPipeline()
    {
        vkDestroyShaderModule(fractalDevice.device(), VertShaderModule, nullptr);
        vkDestroyShaderModule(fractalDevice.device(), FragShaderModule, nullptr);

        vkDestroyPipeline(fractalDevice.device(), GraphicsPipeline, nullptr);
    }

    std::vector<char> FractalPipeline::ReadFile(const std::string &FilePath)
    {
        std::ifstream File{FilePath, std::ios::ate | std::ios::binary};

        if (!File.is_open())
        {
            throw std::runtime_error("Failed To Open File: " + FilePath);
        }
        size_t FileSize = static_cast<size_t>(File.tellg());
        std::vector<char> buffer(FileSize);

        File.seekg(0);
        File.read(buffer.data(), FileSize);

        File.close();
        return buffer;
    }

    void FractalPipeline::DefaultPipelineConfigInfo(PipelineConfigInfo &ConfigInfo, uint32_t Width, uint32_t Height)
    {
        ConfigInfo.Subpass = 0;

        ConfigInfo.InputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        ConfigInfo.InputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        ConfigInfo.InputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

        ConfigInfo.Viewport.x = 0.0f;
        ConfigInfo.Viewport.y = 0.0f;
        ConfigInfo.Viewport.width = static_cast<float>(Width);
        ConfigInfo.Viewport.height = static_cast<float>(Height);
        ConfigInfo.Viewport.minDepth = 0.0f;
        ConfigInfo.Viewport.maxDepth = 1.0f;

        ConfigInfo.Scissor.offset = {0, 0};
        ConfigInfo.Scissor.extent = {Width, Height};

        ConfigInfo.RasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        ConfigInfo.RasterizationInfo.depthClampEnable = VK_FALSE;
        ConfigInfo.RasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
        ConfigInfo.RasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
        ConfigInfo.RasterizationInfo.lineWidth = 1.0f;
        ConfigInfo.RasterizationInfo.cullMode = VK_CULL_MODE_NONE;
        ConfigInfo.RasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
        ConfigInfo.RasterizationInfo.depthBiasEnable = VK_FALSE;
        ConfigInfo.RasterizationInfo.depthBiasConstantFactor = 0.0f; // Optional
        ConfigInfo.RasterizationInfo.depthBiasClamp = 0.0f;          // Optional
        ConfigInfo.RasterizationInfo.depthBiasSlopeFactor = 0.0f;    // Optional

        ConfigInfo.MultisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        ConfigInfo.MultisampleInfo.sampleShadingEnable = VK_FALSE;
        ConfigInfo.MultisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        ConfigInfo.MultisampleInfo.minSampleShading = 1.0f;          // Optional
        ConfigInfo.MultisampleInfo.pSampleMask = nullptr;            // Optional
        ConfigInfo.MultisampleInfo.alphaToCoverageEnable = VK_FALSE; // Optional
        ConfigInfo.MultisampleInfo.alphaToOneEnable = VK_FALSE;      // Optional

        ConfigInfo.ColorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        ConfigInfo.ColorBlendAttachment.blendEnable = VK_FALSE;
        ConfigInfo.ColorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;  // Optional
        ConfigInfo.ColorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
        ConfigInfo.ColorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;             // Optional
        ConfigInfo.ColorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;  // Optional
        ConfigInfo.ColorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
        ConfigInfo.ColorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;             // Optional

        ConfigInfo.ColorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        ConfigInfo.ColorBlendInfo.logicOpEnable = VK_FALSE;
        ConfigInfo.ColorBlendInfo.logicOp = VK_LOGIC_OP_COPY; // Optional
        ConfigInfo.ColorBlendInfo.attachmentCount = 1;
        ConfigInfo.ColorBlendInfo.pAttachments = &ConfigInfo.ColorBlendAttachment;
        ConfigInfo.ColorBlendInfo.blendConstants[0] = 0.0f; // Optional
        ConfigInfo.ColorBlendInfo.blendConstants[1] = 0.0f; // Optional
        ConfigInfo.ColorBlendInfo.blendConstants[2] = 0.0f; // Optional
        ConfigInfo.ColorBlendInfo.blendConstants[3] = 0.0f;

        ConfigInfo.ViewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        ConfigInfo.ViewportInfo.viewportCount = 1;
        ConfigInfo.ViewportInfo.pViewports = &ConfigInfo.Viewport;
        ConfigInfo.ViewportInfo.scissorCount = 1;
        ConfigInfo.ViewportInfo.pScissors = &ConfigInfo.Scissor;

        ConfigInfo.DepthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        ConfigInfo.DepthStencilInfo.depthTestEnable = VK_TRUE;
        ConfigInfo.DepthStencilInfo.depthWriteEnable = VK_TRUE;
        ConfigInfo.DepthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
        ConfigInfo.DepthStencilInfo.depthBoundsTestEnable = VK_FALSE;
        ConfigInfo.DepthStencilInfo.minDepthBounds = 0.0f; // Optional
        ConfigInfo.DepthStencilInfo.maxDepthBounds = 1.0f; // Optional
        ConfigInfo.DepthStencilInfo.stencilTestEnable = VK_FALSE;
        ConfigInfo.DepthStencilInfo.front = {}; // Optional
        ConfigInfo.DepthStencilInfo.back = {};  // Optional
    }

    void FractalPipeline::CreateGraphicsPipeline(
        const PipelineConfigInfo &ConfigInfo,
        const std::string &VertFilePath,
        const std::string &FragFilePath)
    {
        assert(ConfigInfo.PipelineLayout != VK_NULL_HANDLE && "Can't Create Graphics Pipeline: No PipelineLayout provided in ConfigInfo");
        assert(ConfigInfo.RenderPass != VK_NULL_HANDLE && "Can't Create Graphics Pipeline: No RenderPass provided in ConfigInfo");

        auto VertCode = ReadFile(VertFilePath);
        auto FragCode = ReadFile(FragFilePath);

        CreateShaderModule(VertCode, &VertShaderModule);
        CreateShaderModule(FragCode, &FragShaderModule);

        VkPipelineShaderStageCreateInfo ShaderStages[2];
        ShaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        ShaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
        ShaderStages[0].module = VertShaderModule;
        ShaderStages[0].pName = "main";
        ShaderStages[0].flags = 0;
        ShaderStages[0].pNext = nullptr;
        ShaderStages[0].pSpecializationInfo = nullptr;

        ShaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        ShaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        ShaderStages[1].module = FragShaderModule;
        ShaderStages[1].pName = "main";
        ShaderStages[1].flags = 0;
        ShaderStages[1].pNext = nullptr;
        ShaderStages[1].pSpecializationInfo = nullptr;

        auto BindingDescription = FractalModel::Vertex::GetBindingDescriptions();
        auto AttributeDescription = FractalModel::Vertex::GetAttributeDescriptions();

        VkPipelineVertexInputStateCreateInfo VertexInputInfo{};
        VertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        VertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(AttributeDescription.size());
        VertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(BindingDescription.size());
        VertexInputInfo.pVertexAttributeDescriptions = AttributeDescription.data();
        VertexInputInfo.pVertexBindingDescriptions = BindingDescription.data();
        VertexInputInfo.pNext = nullptr;

        VkGraphicsPipelineCreateInfo PipelineInfo{};
        PipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        PipelineInfo.stageCount = 2;
        PipelineInfo.pStages = ShaderStages;
        PipelineInfo.pVertexInputState = &VertexInputInfo;
        PipelineInfo.pInputAssemblyState = &ConfigInfo.InputAssemblyInfo;
        PipelineInfo.pViewportState = &ConfigInfo.ViewportInfo;
        PipelineInfo.pRasterizationState = &ConfigInfo.RasterizationInfo;
        PipelineInfo.pMultisampleState = &ConfigInfo.MultisampleInfo;
        PipelineInfo.pColorBlendState = &ConfigInfo.ColorBlendInfo;
        PipelineInfo.pDepthStencilState = &ConfigInfo.DepthStencilInfo;
        PipelineInfo.pDynamicState = nullptr;
        PipelineInfo.pNext = nullptr;

        PipelineInfo.layout = ConfigInfo.PipelineLayout;
        PipelineInfo.renderPass = ConfigInfo.RenderPass;
        PipelineInfo.subpass = ConfigInfo.Subpass;

        PipelineInfo.basePipelineIndex = -1;
        PipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

        if (vkCreateGraphicsPipelines(fractalDevice.device(), VK_NULL_HANDLE, 1, &PipelineInfo, nullptr, &GraphicsPipeline) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to Create Graphics Pipeline");
        }
    }

    void FractalPipeline::CreateShaderModule(const std::vector<char> &code, VkShaderModule *ShaderModule)
    {
        VkShaderModuleCreateInfo CreateInfo{};
        CreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        CreateInfo.codeSize = code.size();
        CreateInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());

        if (vkCreateShaderModule(fractalDevice.device(), &CreateInfo, nullptr, ShaderModule) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed To Create Shader Module");
        }
    }

    void FractalPipeline::Bind(VkCommandBuffer CommandBuffer)
    {
        vkCmdBindPipeline(CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, GraphicsPipeline);
    }

}