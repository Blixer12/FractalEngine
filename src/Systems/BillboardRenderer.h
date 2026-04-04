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
    class BillboardRenderSystem
    {

    public:
        BillboardRenderSystem(FractalDevice &Device, VkRenderPass RenderPass, VkDescriptorSetLayout GlobalSetLayout);
        ~BillboardRenderSystem();

        BillboardRenderSystem(const BillboardRenderSystem &) = delete;
        BillboardRenderSystem &operator=(const BillboardRenderSystem &) = delete;

        void Render(FrameInfo &FrameInfo);

    private:
        void CreatePipelineLayout(VkDescriptorSetLayout GlobalSetLayout);
        void CreatePipeline(VkRenderPass RenderPass);

        FractalDevice &FractalAppDevice;

        std::unique_ptr<FractalPipeline> BillboardRenderSystemPipeline;
        VkPipelineLayout PipelineLayout;
    };
}