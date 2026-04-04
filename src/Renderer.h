#pragma once

// Standard Library
#include <cassert>
#include <memory>
#include <vector>

#include "Device.h"
#include "Swapchain.h"
#include "Window.h"

namespace FractalEngine
{
    class FractalRenderer
    {

    public:
        FractalRenderer(FractalWindow &Window, FractalDevice &Device);
        ~FractalRenderer();

        FractalRenderer(const FractalRenderer &) = delete;
        FractalRenderer &operator=(const FractalRenderer &) = delete;

        VkRenderPass GetSwapchainRenderPass() const { return FractalAppSwapChain->getRenderPass(); }
        float GetAspectRatio() const { return FractalAppSwapChain->extentAspectRatio(); }
        bool IsFrameInProgress() const { return IsFrameStarted; }

        VkCommandBuffer GetCurrentCommandBuffer() const
        {
            assert(IsFrameStarted && "Can't Get CommandBuffer when Frame is Not in Progress");
            return CommandBuffers[FrameIndex];
        }

        int GetFrameIndex() const
        {
            assert(IsFrameStarted && "Can't Get Frame Index when Frame is Not in Progress");
            return FrameIndex;
        }

        VkCommandBuffer BeginFrame();
        void EndFrame();
        void BeginSwapChainRenderPass(VkCommandBuffer CommandBuffer);
        void EndSwapChainRenderPass(VkCommandBuffer CommandBuffer);

    private:
        void CreateCommandBuffers();
        void FreeCommandBuffers();
        void RecreateSwapChain();

        FractalWindow &FractalAppWindow;
        FractalDevice &FractalAppDevice;

        std::unique_ptr<FractalSwapChain> FractalAppSwapChain;
        std::vector<VkCommandBuffer> CommandBuffers;

        uint32_t CurrentImageIndex;
        int FrameIndex = 0;
        bool IsFrameStarted = false;
    };
}