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
#include "Camera.h"

namespace FractalEngine
{

    struct PushConstantData
    {
        glm::mat4 ModelMatrix{1.f};
        glm::mat4 NormalMatrix{1.f};
    };

    FractalRenderSystem::FractalRenderSystem(FractalDevice &Device, VkRenderPass RenderPass, VkDescriptorSetLayout GlobalSetLayout) : FractalAppDevice{Device}
    {
        CreatePipelineLayout(GlobalSetLayout);
        CreatePipeline(RenderPass);
    }

    FractalRenderSystem::~FractalRenderSystem()
    {
        vkDeviceWaitIdle(FractalAppDevice.device());
        vkDestroyPipelineLayout(FractalAppDevice.device(), PipelineLayout, nullptr);
    }

    void FractalRenderSystem::CreatePipelineLayout(VkDescriptorSetLayout GlobalSetLayout)
    {

        VkPushConstantRange PushConstantRange{};
        PushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        PushConstantRange.offset = 0;
        PushConstantRange.size = sizeof(PushConstantData);

        std::vector<VkDescriptorSetLayout> DescriptorSetLayouts{GlobalSetLayout};

        VkPipelineLayoutCreateInfo PipelineLayoutInfo{};
        PipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        PipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(DescriptorSetLayouts.size());
        PipelineLayoutInfo.pSetLayouts = DescriptorSetLayouts.data();
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

    void FractalRenderSystem::RenderGameObjects(FrameInfo &FrameInfo)
    {
        FractalRenderSystemPipeline->Bind(FrameInfo.CommandBuffer);

        vkCmdBindDescriptorSets(
            FrameInfo.CommandBuffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            PipelineLayout,
            0,
            1,
            &FrameInfo.GlobalDescriptorSet,
            0,
            nullptr);

        for (auto &KeyValue : FrameInfo.GameObjects)
        {
            auto &Object = KeyValue.second;
            if (Object.Model == nullptr)
                continue;
            PushConstantData Push{};
            Push.ModelMatrix = Object.Transform.TransformMatrix();
            Push.NormalMatrix = Object.Transform.NormalMatrix();

            vkCmdPushConstants(FrameInfo.CommandBuffer, PipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(PushConstantData), &Push);

            Object.Model->Bind(FrameInfo.CommandBuffer);
            Object.Model->Draw(FrameInfo.CommandBuffer);
        }
    }

}