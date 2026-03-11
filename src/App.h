#pragma once

// Standard Library
#include <memory>
#include <vector>

#include "Pipeline.h"
#include "Device.h"
#include "Swapchain.h"
#include "Window.h"
#include "Model.h"

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
        void LoadModels();
        void CreatePipelineLayout();
        void CreatePipeline();
        void CreateCommandBuffers();
        void Render();
        void DrawFrame();
        void RecreateSwapChain();
        void RecordCommandBuffer(int ImageIndex);

        FractalWindow FractalAppWindow{Width, Height, "Fractal Engine"};
        FractalDevice FractalAppDevice{FractalAppWindow};

        std::unique_ptr<FractalSwapChain> FractalAppSwapChain;
        std::unique_ptr<FractalPipeline> FractalAppPipeline;
        VkPipelineLayout PipelineLayout;
        std::vector<VkCommandBuffer> CommandBuffers;
        std::unique_ptr<FractalModel> FractalAppModel;
    };
}