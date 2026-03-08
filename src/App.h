#pragma once

// Standard Library
#include <memory>
#include <vector>

#include "Pipeline.h"
#include "Device.h"
#include "Swapchain.h"
#include "Window.h"

namespace FractalEngine
{
    class App
    {

    public:
        static constexpr int Width = 1280;
        static constexpr int Height = 720;

        App();
        ~App();

        App(const App &) = delete;
        App &operator=(const App &) = delete;

        void Run();

    private:
        void CreatePipelineLayout();
        void CreatePipeline();
        void CreateCommandBuffers();
        void Render();
        void DrawFrame();

        FractalWindow FractalAppWindow{Width, Height, "Fractal Engine"};
        FractalDevice FractalAppDevice{FractalAppWindow};
        FractalSwapChain FractalAppSwapChain{FractalAppDevice, FractalAppWindow.GetExtent()};

        std::unique_ptr<FractalPipeline> FractalAppPipeline;
        VkPipelineLayout PipelineLayout;
        std::vector<VkCommandBuffer> CommandBuffers;
    };
}