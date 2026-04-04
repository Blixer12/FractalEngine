// Standard Library
#include <stdexcept>
#include <iostream>
#include <array>

// Librarys
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include "BillboardRenderer.h"
#include "Camera.h"

namespace FractalEngine
{
    BillboardRenderSystem::BillboardRenderSystem(FractalDevice &Device, VkRenderPass RenderPass, VkDescriptorSetLayout GlobalSetLayout) : FractalAppDevice{Device}
    {
        CreatePipelineLayout(GlobalSetLayout);
        CreatePipeline(RenderPass);
    }

    BillboardRenderSystem::~BillboardRenderSystem()
    {
        vkDeviceWaitIdle(FractalAppDevice.device());
        vkDestroyPipelineLayout(FractalAppDevice.device(), PipelineLayout, nullptr);
    }

    void BillboardRenderSystem::CreatePipelineLayout(VkDescriptorSetLayout GlobalSetLayout)
    {

        // VkPushConstantRange PushConstantRange{};
        // PushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        // PushConstantRange.offset = 0;
        // PushConstantRange.size = sizeof(PushConstantData);

        std::vector<VkDescriptorSetLayout> DescriptorSetLayouts{GlobalSetLayout};

        VkPipelineLayoutCreateInfo PipelineLayoutInfo{};
        PipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        PipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(DescriptorSetLayouts.size());
        PipelineLayoutInfo.pSetLayouts = DescriptorSetLayouts.data();
        PipelineLayoutInfo.pushConstantRangeCount = 0;
        PipelineLayoutInfo.pPushConstantRanges = nullptr;

        if (vkCreatePipelineLayout(FractalAppDevice.device(), &PipelineLayoutInfo, nullptr, &PipelineLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to Create Pipeline Layout");
        }
    }

    void BillboardRenderSystem::CreatePipeline(VkRenderPass RenderPass)
    {
        assert(PipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

        PipelineConfigInfo PipelineConfig{};

        FractalPipeline::DefaultPipelineConfigInfo(PipelineConfig);

        PipelineConfig.RenderPass = RenderPass;
        PipelineConfig.PipelineLayout = PipelineLayout;
        BillboardRenderSystemPipeline = std::make_unique<FractalPipeline>(
            FractalAppDevice,
            PipelineConfig,
            "Assets/Shaders/PointLight.vert.spv",
            "Assets/Shaders/PointLight.frag.spv");
    }

    void BillboardRenderSystem::Render(FrameInfo &FrameInfo)
    {
        BillboardRenderSystemPipeline->Bind(FrameInfo.CommandBuffer);

        vkCmdBindDescriptorSets(
            FrameInfo.CommandBuffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            PipelineLayout,
            0,
            1,
            &FrameInfo.GlobalDescriptorSet,
            0,
            nullptr);

        vkCmdDraw(FrameInfo.CommandBuffer, 6, 1, 0, 0);
    }

}