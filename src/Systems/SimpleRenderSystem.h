#pragma once

// Standard Library
#include <memory>
#include <vector>

#include "Camera.h"
#include "Device.h"
#include "FrameInfo.h"
#include "GameObject.h"
#include "Pipeline.h"

namespace FractalEngine
{
    class FractalRenderSystem
    {

    public:
        FractalRenderSystem(FractalDevice &Device, VkRenderPass RenderPass, VkDescriptorSetLayout GlobalSetLayout);
        ~FractalRenderSystem();

        FractalRenderSystem(const FractalRenderSystem &) = delete;
        FractalRenderSystem &operator=(const FractalRenderSystem &) = delete;

        void RenderGameObjects(FrameInfo &FrameInfo);

    private:
        void CreatePipelineLayout(VkDescriptorSetLayout GlobalSetLayout);
        void CreatePipeline(VkRenderPass RenderPass);

        FractalDevice &FractalAppDevice;

        std::unique_ptr<FractalPipeline> FractalRenderSystemPipeline;
        VkPipelineLayout PipelineLayout;
    };
}