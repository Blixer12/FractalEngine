// Standard Library
#include <stdexcept>
#include <iostream>
#include <array>

#include "Renderer.h"

namespace FractalEngine
{
    FractalRenderer::FractalRenderer(FractalWindow &Window, FractalDevice &Device) : FractalAppWindow{Window}, FractalAppDevice{Device}
    {
        RecreateSwapChain();
        CreateCommandBuffers();
    }

    FractalRenderer::~FractalRenderer()
    {
        vkDeviceWaitIdle(FractalAppDevice.device());

        FreeCommandBuffers();
    }

    void FractalRenderer::CreateCommandBuffers()
    {
        CommandBuffers.resize(FractalSwapChain::MAX_FRAMES_IN_FLIGHT);

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

    void FractalRenderer::RecreateSwapChain()
    {
        auto Extent = FractalAppWindow.GetExtent();
        if (Extent.width == 0 || Extent.height == 0)
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
            std::shared_ptr<FractalSwapChain> OldSwapChain = std::move(FractalAppSwapChain);
            FractalAppSwapChain = std::make_unique<FractalSwapChain>(FractalAppDevice, Extent, OldSwapChain);

            if (!OldSwapChain->CompareSwapFormats(*FractalAppSwapChain.get()))
            {
                throw std::runtime_error("Swapchain image/depth format changed");
            }
        }

        // Brb
    }

    void FractalRenderer::FreeCommandBuffers()
    {
        vkFreeCommandBuffers(FractalAppDevice.device(), FractalAppDevice.getCommandPool(), static_cast<uint32_t>(CommandBuffers.size()), CommandBuffers.data());
        CommandBuffers.clear();
    }

    VkCommandBuffer FractalRenderer::BeginFrame()
    {
        assert(!IsFrameStarted && "Can't call BeginFrame while already in progress");

        auto Result = FractalAppSwapChain->acquireNextImage(&CurrentImageIndex);

        if (Result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            RecreateSwapChain();
            return nullptr;
        }

        if (Result != VK_SUCCESS && Result != VK_SUBOPTIMAL_KHR)
        {
            throw std::runtime_error("Failed Aquiring Swapchain Image");
        }

        IsFrameStarted = true;

        auto CommandBuffer = GetCurrentCommandBuffer();

        VkCommandBufferBeginInfo BeginInfo{};
        BeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(CommandBuffer, &BeginInfo) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to Begin Recording Command Buffer");
        }

        return CommandBuffer;
    }
    void FractalRenderer::EndFrame()
    {
        assert(IsFrameStarted && "Can't call EndFrame while Frame isn't in progress");
        auto CommandBuffer = GetCurrentCommandBuffer();

        if (vkEndCommandBuffer(CommandBuffer) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to Record Command Buffer");
        }

        auto Result = FractalAppSwapChain->submitCommandBuffers(&CommandBuffer, &CurrentImageIndex);

        bool ShouldRecreateSwapChain = Result == VK_ERROR_OUT_OF_DATE_KHR || Result == VK_SUBOPTIMAL_KHR || FractalAppWindow.WasWindowResized();

        if (ShouldRecreateSwapChain)
        {
            FractalAppWindow.ResetWindowResizedFlag();
            RecreateSwapChain();
            IsFrameStarted = false;
            return;
        }
        if (Result != VK_SUCCESS)
        {
            std::cout << Result << std::endl;
            throw std::runtime_error("Failed Presenting Swapchain Image");
        }

        IsFrameStarted = false;
        FrameIndex = (FrameIndex + 1) % FractalSwapChain::MAX_FRAMES_IN_FLIGHT;
    }
    void FractalRenderer::BeginSwapChainRenderPass(VkCommandBuffer CommandBuffer)
    {
        assert(IsFrameStarted && "Can't call BeginSwapChain if frame is not in progress");
        assert(CommandBuffer == GetCurrentCommandBuffer() && "Can't begin render pass on command buffer from a different frame");

        VkRenderPassBeginInfo RenderPassInfo{};
        RenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        RenderPassInfo.renderPass = FractalAppSwapChain->getRenderPass();
        RenderPassInfo.framebuffer = FractalAppSwapChain->getFrameBuffer(CurrentImageIndex);

        RenderPassInfo.renderArea.offset = {0, 0};
        RenderPassInfo.renderArea.extent = FractalAppSwapChain->getSwapChainExtent();

        std::array<VkClearValue, 2> ClearValues{};
        ClearValues[0].color = {0.01f, 0.01f, 0.01f, 1};
        ClearValues[1].depthStencil = {1, 0};
        RenderPassInfo.clearValueCount = static_cast<uint32_t>(ClearValues.size());
        RenderPassInfo.pClearValues = ClearValues.data();

        vkCmdBeginRenderPass(CommandBuffer, &RenderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport Viewport{};
        Viewport.x = 0.0f;
        Viewport.y = 0.0f;
        Viewport.width = static_cast<float>(FractalAppSwapChain->getSwapChainExtent().width);
        Viewport.height = static_cast<float>(FractalAppSwapChain->getSwapChainExtent().height);
        Viewport.minDepth = 0.0f;
        Viewport.maxDepth = 1.0f;
        VkRect2D Scissor{{0, 0}, FractalAppSwapChain->getSwapChainExtent()};
        vkCmdSetViewport(CommandBuffer, 0, 1, &Viewport);
        vkCmdSetScissor(CommandBuffer, 0, 1, &Scissor);
    }
    void FractalRenderer::EndSwapChainRenderPass(VkCommandBuffer CommandBuffer)
    {
        assert(IsFrameStarted && "Can't call EndSwapChain if frame is not in progress");
        assert(CommandBuffer == GetCurrentCommandBuffer() && "Can't End render pass on command buffer from a different frame");

        vkCmdEndRenderPass(CommandBuffer);
    }

}