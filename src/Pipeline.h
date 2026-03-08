#pragma once

// Standard Library
#include <string>
#include <vector>

#include "Device.h"

namespace FractalEngine
{
    struct PipelineConfigInfo
    {

        PipelineConfigInfo(const PipelineConfigInfo &) = delete;
        PipelineConfigInfo &operator=(const PipelineConfigInfo &) = delete;

        VkViewport Viewport;
        VkRect2D Scissor;
        VkPipelineInputAssemblyStateCreateInfo InputAssemblyInfo;
        VkPipelineRasterizationStateCreateInfo RasterizationInfo;
        VkPipelineMultisampleStateCreateInfo MultisampleInfo;
        VkPipelineColorBlendAttachmentState ColorBlendAttachment;
        VkPipelineColorBlendStateCreateInfo ColorBlendInfo;
        VkPipelineDepthStencilStateCreateInfo DepthStencilInfo;
        VkPipelineViewportStateCreateInfo ViewportInfo;
        VkPipelineLayout PipelineLayout = nullptr;
        VkRenderPass RenderPass = nullptr;
        uint32_t Subpass = 0;
    };

    class FractalPipeline
    {
    public:
        FractalPipeline(
            FractalDevice &fractalDevice,
            const PipelineConfigInfo &ConfigInfo,
            const std::string &VertFilePath,
            const std::string &FragFilePath);

        ~FractalPipeline();

        FractalPipeline(const std::string &FilePath) = delete;
        FractalPipeline operator=(const std::string &FilePath) = delete;

        void Bind(VkCommandBuffer CommandBuffer);

        static void DefaultPipelineConfigInfo(PipelineConfigInfo &ConfigInfo, uint32_t Width, uint32_t Height);

    private:
        static std::vector<char> ReadFile(const std::string &FilePath);

        void CreateGraphicsPipeline(
            const PipelineConfigInfo &ConfigInfo,
            const std::string &VertFilePath,
            const std::string &FragFilePath);

        void CreateShaderModule(const std::vector<char> &code, VkShaderModule *ShaderModule);

        FractalDevice &fractalDevice;
        VkPipeline GraphicsPipeline;
        VkShaderModule VertShaderModule;
        VkShaderModule FragShaderModule;
    };
}