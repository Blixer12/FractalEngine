// Standard Library
#include <stdexcept>
#include <array>

#include "App.h"

namespace FractalEngine
{

    App::App()
    {
        LoadModels();
        CreatePipelineLayout();
        CreatePipeline();
        CreateCommandBuffers();
    }

    App::~App()
    {
        vkDeviceWaitIdle(FractalAppDevice.device());

        vkDestroyPipelineLayout(FractalAppDevice.device(), PipelineLayout, nullptr);
    }

    void App::Run()
    {
        while (!FractalAppWindow.ShouldClose())
        {
            glfwPollEvents();
            DrawFrame();
        }

        vkDeviceWaitIdle(FractalAppDevice.device());
    }

    void App::LoadModels()
    {
        std::vector<FractalModel::Vertex> Vertices{
            {{0, -0.5f}, {1, 0, 0, 1}},
            {{0.5f, 0.5f}, {0, 1, 0, 1}},
            {{-0.5f, 0.5f}, {0, 0, 1, 1}}};

        FractalAppModel = std::make_unique<FractalModel>(FractalAppDevice, Vertices);
    }

    void App::CreatePipelineLayout()
    {
        VkPipelineLayoutCreateInfo PipelineLayoutInfo{};
        PipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        PipelineLayoutInfo.setLayoutCount = 0;
        PipelineLayoutInfo.pSetLayouts = nullptr;
        PipelineLayoutInfo.pushConstantRangeCount = 0;
        PipelineLayoutInfo.pPushConstantRanges = nullptr;

        if (vkCreatePipelineLayout(FractalAppDevice.device(), &PipelineLayoutInfo, nullptr, &PipelineLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to Create Pipeline Layout");
        }
    }

    void App::CreatePipeline()
    {
        PipelineConfigInfo PipelineConfig{};

        FractalPipeline::DefaultPipelineConfigInfo(PipelineConfig, FractalAppSwapChain.width(), FractalAppSwapChain.height());

        PipelineConfig.RenderPass = FractalAppSwapChain.getRenderPass();
        PipelineConfig.PipelineLayout = PipelineLayout;
        FractalAppPipeline = std::make_unique<FractalPipeline>(
            FractalAppDevice,
            PipelineConfig,
            "Assets/Shaders/Simple_Shader.vert.spv",
            "Assets/Shaders/Simple_Shader.frag.spv");
    }

    void App::CreateCommandBuffers()
    {
        CommandBuffers.resize(FractalAppSwapChain.imageCount());

        VkCommandBufferAllocateInfo AllocateInfo{};
        AllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        AllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        AllocateInfo.commandPool = FractalAppDevice.getCommandPool();
        AllocateInfo.commandBufferCount = static_cast<uint32_t>(CommandBuffers.size());

        if (vkAllocateCommandBuffers(FractalAppDevice.device(), &AllocateInfo, CommandBuffers.data()) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed Allocating Command Buffers");
        }

        for (int i = 0; i < CommandBuffers.size(); i++)
        {
            VkCommandBufferBeginInfo BeginInfo{};
            BeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

            if (vkBeginCommandBuffer(CommandBuffers[i], &BeginInfo) != VK_SUCCESS)
            {
                throw std::runtime_error("Failed to Begin Recording Command Buffer");
            }

            VkRenderPassBeginInfo RenderPassInfo{};
            RenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            RenderPassInfo.renderPass = FractalAppSwapChain.getRenderPass();
            RenderPassInfo.framebuffer = FractalAppSwapChain.getFrameBuffer(i);

            RenderPassInfo.renderArea.offset = {0, 0};
            RenderPassInfo.renderArea.extent = FractalAppSwapChain.getSwapChainExtent();

            std::array<VkClearValue, 2> ClearValues{};
            ClearValues[0].color = {0.1f, 0.1f, 0.1f, 1};
            ClearValues[1].depthStencil = {1, 0};
            RenderPassInfo.clearValueCount = static_cast<uint32_t>(ClearValues.size());
            RenderPassInfo.pClearValues = ClearValues.data();

            vkCmdBeginRenderPass(CommandBuffers[i], &RenderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

            FractalAppPipeline->Bind(CommandBuffers[i]);
            FractalAppModel->Bind(CommandBuffers[i]);
            FractalAppModel->Draw(CommandBuffers[i]);

            vkCmdEndRenderPass(CommandBuffers[i]);
            if (vkEndCommandBuffer(CommandBuffers[i]) != VK_SUCCESS)
            {
                throw std::runtime_error("Failed to Record Command Buffer");
            }
        }
    }

    void App::Render() {}

    void App::DrawFrame()
    {
        uint32_t ImageIndex;
        auto Result = FractalAppSwapChain.acquireNextImage(&ImageIndex);

        if (Result != VK_SUCCESS && Result != VK_SUBOPTIMAL_KHR)
        {
            throw std::runtime_error("Failed Aquiring Swapchain Image");
        }

        Result = FractalAppSwapChain.submitCommandBuffers(&CommandBuffers[ImageIndex], &ImageIndex);
        if (Result != VK_SUCCESS)
        {
            throw std::runtime_error("Failed Presenting Swapchain Image");
        }
    }

}