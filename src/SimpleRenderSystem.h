#pragma once

// Standard Library
#include <memory>
#include <vector>

#include "Device.h"
#include "GameObject.h"
#include "Pipeline.h"

namespace FractalEngine
{
    class FractalRenderSystem
    {

    public:
        FractalRenderSystem(FractalDevice &Device, VkRenderPass RenderPass);
        ~FractalRenderSystem();

        FractalRenderSystem(const FractalRenderSystem &) = delete;
        FractalRenderSystem &operator=(const FractalRenderSystem &) = delete;

        void RenderGameObjects(VkCommandBuffer CommandBuffer, std::vector<FractalGameObject> &GameObjects);

    private:
        void CreatePipelineLayout();
        void CreatePipeline(VkRenderPass RenderPass);

        FractalDevice &FractalAppDevice;

        std::unique_ptr<FractalPipeline> FractalRenderSystemPipeline;
        VkPipelineLayout PipelineLayout;
    };
}