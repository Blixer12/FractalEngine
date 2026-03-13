// Standard Library
#include <stdexcept>
#include <iostream>
#include <array>

// Librarys
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include "SimpleRenderSystem.h"

namespace FractalEngine
{

    struct PushConstantData
    {
        glm::mat2 Transform{1.f};
        glm::vec4 Color;
        alignas(16) glm::vec2 Offset;
    };

    FractalRenderSystem::FractalRenderSystem(FractalDevice &Device, VkRenderPass RenderPass) : FractalAppDevice{Device}
    {
        CreatePipelineLayout();
        CreatePipeline(RenderPass);
    }

    FractalRenderSystem::~FractalRenderSystem()
    {
        vkDeviceWaitIdle(FractalAppDevice.device());

        vkDestroyPipelineLayout(FractalAppDevice.device(), PipelineLayout, nullptr);
    }

    void FractalRenderSystem::CreatePipelineLayout()
    {

        VkPushConstantRange PushConstantRange{};
        PushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        PushConstantRange.offset = 0;
        PushConstantRange.size = sizeof(PushConstantData);

        VkPipelineLayoutCreateInfo PipelineLayoutInfo{};
        PipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        PipelineLayoutInfo.setLayoutCount = 0;
        PipelineLayoutInfo.pSetLayouts = nullptr;
        PipelineLayoutInfo.pushConstantRangeCount = 1;
        PipelineLayoutInfo.pPushConstantRanges = &PushConstantRange;

        if (vkCreatePipelineLayout(FractalAppDevice.device(), &PipelineLayoutInfo, nullptr, &PipelineLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to Create Pipeline Layout");
        }
    }

    void FractalRenderSystem::CreatePipeline(VkRenderPass RenderPass)
    {
        assert(PipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

        PipelineConfigInfo PipelineConfig{};

        FractalPipeline::DefaultPipelineConfigInfo(PipelineConfig);

        PipelineConfig.RenderPass = RenderPass;
        PipelineConfig.PipelineLayout = PipelineLayout;
        FractalRenderSystemPipeline = std::make_unique<FractalPipeline>(
            FractalAppDevice,
            PipelineConfig,
            "Assets/Shaders/Simple_Shader.vert.spv",
            "Assets/Shaders/Simple_Shader.frag.spv");
    }

    void FractalRenderSystem::RenderGameObjects(VkCommandBuffer CommandBuffer, std::vector<FractalGameObject> &GameObjects)
    {
        int i = 0;

        for (auto &Object : GameObjects)
        {
            i += 1;
            Object.Transform2D.Rotation = glm::mod(Object.Transform2D.Rotation + 0.2f * i, 2.f * glm::two_pi<float>());
        }

        FractalRenderSystemPipeline->Bind(CommandBuffer);

        for (auto &Object : GameObjects)
        {
            PushConstantData Push{};
            Push.Offset = Object.Transform2D.Translation;
            Push.Color = Object.Color;
            Push.Transform = Object.Transform2D.TransformMatrix();

            vkCmdPushConstants(CommandBuffer, PipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(PushConstantData), &Push);

            Object.Model->Bind(CommandBuffer);
            Object.Model->Draw(CommandBuffer);
        }
    }

}