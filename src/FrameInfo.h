#pragma once

#include "Camera.h"
#include "GameObject.h"

// Librarys
#include <vulkan/vulkan.h>

namespace FractalEngine
{
    struct FrameInfo
    {
        int FrameIndex;
        float FrameTime;
        VkCommandBuffer CommandBuffer;
        FractalCamera &Camera;
        VkDescriptorSet GlobalDescriptorSet;
        FractalGameObject::Map &GameObjects;
    };
}