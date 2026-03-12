// Standard Library
#include <stdexcept>
#include <iostream>
#include <array>

// Librarys
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include "App.h"

namespace FractalEngine
{

    struct PushConstantData
    {
        glm::mat2 Transform{1.f};
        glm::vec4 Color;
        alignas(16) glm::vec2 Offset;
    };

    App::App()
    {
        LoadGameObjects();
        CreatePipelineLayout();
        RecreateSwapChain();
        CreateCommandBuffers();
    }

    App::~App()
    {
        vkDeviceWaitIdle(FractalAppDevice.device());

        vkDestroyPipelineLayout(FractalAppDevice.device(), PipelineLayout, nullptr);
    }

    void App::Run()
    {
        std::cout << "Max push constant size = " << FractalAppDevice.properties.limits.maxPushConstantsSize << std::endl;

        while (!FractalAppWindow.ShouldClose())
        {
            glfwPollEvents();
            DrawFrame();
        }

        vkDeviceWaitIdle(FractalAppDevice.device());
    }

    void App::LoadGameObjects()
    {
        std::vector<FractalModel::Vertex> Vertices{
            {{0, -0.5f}, {1, 0, 0, 1}},
            {{0.5f, 0.5f}, {0, 1, 0, 1}},
            {{-0.5f, 0.5f}, {0, 0, 1, 1}}};

        auto FractalAppModel = std::make_shared<FractalModel>(FractalAppDevice, Vertices);

        auto Triangle = FractalGameObject::CreateGameObject();
        Triangle.Model = FractalAppModel;
        Triangle.Color = {.1f, .8f, .1f, 1};
        Triangle.Transform2D.Translation.x = .2f;
        Triangle.Transform2D.Scale = {2, 0.5f};
        Triangle.Transform2D.Rotation = 0.25f * glm::two_pi<float>();

        GameObjects.push_back(std::move(Triangle));
    }

    void App::CreatePipelineLayout()
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

    void App::CreatePipeline()
    {
        assert(FractalAppSwapChain != nullptr && "Cannot create pipeline before swap chain");
        assert(PipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

        PipelineConfigInfo PipelineConfig{};

        FractalPipeline::DefaultPipelineConfigInfo(PipelineConfig);

        PipelineConfig.RenderPass = FractalAppSwapChain->getRenderPass();
        PipelineConfig.PipelineLayout = PipelineLayout;
        FractalAppPipeline = std::make_unique<FractalPipeline>(
            FractalAppDevice,
            PipelineConfig,
            "Assets/Shaders/Simple_Shader.vert.spv",
            "Assets/Shaders/Simple_Shader.frag.spv");
    }

    void App::CreateCommandBuffers()
    {
        CommandBuffers.resize(FractalAppSwapChain->imageCount());

        VkCommandBufferAllocateInfo AllocateInfo{};
        AllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        AllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        AllocateInfo.commandPool = FractalAppDevice.getCommandPool();
        AllocateInfo.commandBufferCount = static_cast<uint32_t>(CommandBuffers.size());

        if (vkAllocateCommandBuffers(FractalAppDevice.device(), &AllocateInfo, CommandBuffers.data()) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed Allocating Command Buffers");
        }
    }

    void App::RecreateSwapChain()
    {
        auto Extent = FractalAppWindow.GetExtent();
        while (Extent.width == 0 || Extent.height == 0)
        {
            glfwWaitEvents();
            Extent = FractalAppWindow.GetExtent();
        }

        vkDeviceWaitIdle(FractalAppDevice.device());

        if (FractalAppSwapChain == nullptr)
        {
            FractalAppSwapChain = std::make_unique<FractalSwapChain>(FractalAppDevice, Extent);
        }
        else
        {
            FractalAppSwapChain = std::make_unique<FractalSwapChain>(FractalAppDevice, Extent, std::move(FractalAppSwapChain));
            if (FractalAppSwapChain->imageCount() != CommandBuffers.size())
            {
                FreeCommandBuffers();
                CreateCommandBuffers();
            }
        }

        CreatePipeline();
    }

    void App::FreeCommandBuffers()
    {
        vkFreeCommandBuffers(FractalAppDevice.device(), FractalAppDevice.getCommandPool(), static_cast<uint32_t>(CommandBuffers.size()), CommandBuffers.data());
        CommandBuffers.clear();
    }

    void App::RecordCommandBuffer(int ImageIndex)
    {
        VkCommandBufferBeginInfo BeginInfo{};
        BeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(CommandBuffers[ImageIndex], &BeginInfo) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to Begin Recording Command Buffer");
        }

        VkRenderPassBeginInfo RenderPassInfo{};
        RenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        RenderPassInfo.renderPass = FractalAppSwapChain->getRenderPass();
        RenderPassInfo.framebuffer = FractalAppSwapChain->getFrameBuffer(ImageIndex);

        RenderPassInfo.renderArea.offset = {0, 0};
        RenderPassInfo.renderArea.extent = FractalAppSwapChain->getSwapChainExtent();

        std::array<VkClearValue, 2> ClearValues{};
        ClearValues[0].color = {0.01f, 0.01f, 0.01f, 1};
        ClearValues[1].depthStencil = {1, 0};
        RenderPassInfo.clearValueCount = static_cast<uint32_t>(ClearValues.size());
        RenderPassInfo.pClearValues = ClearValues.data();

        vkCmdBeginRenderPass(CommandBuffers[ImageIndex], &RenderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport Viewport{};
        Viewport.x = 0.0f;
        Viewport.y = 0.0f;
        Viewport.width = static_cast<float>(FractalAppSwapChain->getSwapChainExtent().width);
        Viewport.height = static_cast<float>(FractalAppSwapChain->getSwapChainExtent().height);
        Viewport.minDepth = 0.0f;
        Viewport.maxDepth = 1.0f;
        VkRect2D Scissor{{0, 0}, FractalAppSwapChain->getSwapChainExtent()};
        vkCmdSetViewport(CommandBuffers[ImageIndex], 0, 1, &Viewport);
        vkCmdSetScissor(CommandBuffers[ImageIndex], 0, 1, &Scissor);

        RenderGameObjects(CommandBuffers[ImageIndex]);

        vkCmdEndRenderPass(CommandBuffers[ImageIndex]);
        if (vkEndCommandBuffer(CommandBuffers[ImageIndex]) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to Record Command Buffer");
        }
    }

    void App::RenderGameObjects(VkCommandBuffer CommandBuffer)
    {
        int i = 0;

        for (auto &Object : GameObjects)
        {
            i += 1;
            Object.Transform2D.Rotation = glm::mod(Object.Transform2D.Rotation + 0.2f * i, 2.f * glm::two_pi<float>());
        }

        FractalAppPipeline->Bind(CommandBuffer);

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

    void App::Render() {}

    void App::DrawFrame()
    {
        uint32_t ImageIndex;
        auto Result = FractalAppSwapChain->acquireNextImage(&ImageIndex);

        if (Result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            RecreateSwapChain();
            return;
        }

        if (Result != VK_SUCCESS && Result != VK_SUBOPTIMAL_KHR)
        {
            throw std::runtime_error("Failed Aquiring Swapchain Image");
        }

        RecordCommandBuffer(ImageIndex);

        Result = FractalAppSwapChain->submitCommandBuffers(&CommandBuffers[ImageIndex], &ImageIndex);

        if (Result == VK_ERROR_OUT_OF_DATE_KHR || Result == VK_SUBOPTIMAL_KHR || FractalAppWindow.WasWindowResized())
        {
            FractalAppWindow.ResetWindowResizedFlag();
            RecreateSwapChain();
            return;
        }
        if (Result != VK_SUCCESS)
        {
            throw std::runtime_error("Failed Presenting Swapchain Image");
        }
    }

}