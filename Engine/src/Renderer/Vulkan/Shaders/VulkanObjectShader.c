#include "VulkanObjectShader.h"

#include "Core/Logger.h"
#include "Core/Memory.h"

#include "../VulkanShaderUtils.h"
#include "../VulkanPipeline.h"

#include "Math/MathDef.h"

#define BUILTIN_SHADER_NAME_OBJECT "Builtin.ObjectShader"


Bool8 VulkanObjectShaderCreate(VulkanContext* Context, VulkanObjectShader* Shader) {
    // Shader module init per stage.
    char StageTypeStrings[OBJECT_SHADER_STAGE_COUNT][5] = {"vert", "frag"};
    VkShaderStageFlagBits StageTypes[OBJECT_SHADER_STAGE_COUNT] = {VK_SHADER_STAGE_VERTEX_BIT, VK_SHADER_STAGE_FRAGMENT_BIT};

    for (UInt32 i = 0; i < OBJECT_SHADER_STAGE_COUNT; i++) {
        if (!CreateShaderModule(Context, BUILTIN_SHADER_NAME_OBJECT, StageTypeStrings[i], StageTypes[i], i, Shader->Stages)) {
            FLERROR("Unable to create %s shader module for '%s'.", StageTypeStrings[i], BUILTIN_SHADER_NAME_OBJECT);
            return false;
        }
    }

    // TODO: Descriptors

    // Pipeline Generation
    VkViewport Viewport;
    Viewport.x = 0.0f;
    Viewport.y = (Float32)Context->FramebufferHeight;
    Viewport.width = (Float32)Context->FramebufferWidth;
    Viewport.height = -(Float32)Context->FramebufferHeight;
    Viewport.minDepth = 0.0f;
    Viewport.maxDepth = 1.0f;

    VkRect2D Scissor;
    Scissor.offset.x = Scissor.offset.y = 0;
    Scissor.extent.width = Context->FramebufferWidth;
    Scissor.extent.height = Context->FramebufferHeight;

    // Attributes
    UInt32 Offset = 0;
    constexpr Int32 AttributeCount = 1;
    VkVertexInputAttributeDescription AttributeDescriptions[AttributeCount];
    // Position
    VkFormat Formats[AttributeCount] = {
        VK_FORMAT_R32G32B32_SFLOAT
    };

    UInt64 Sizes[AttributeCount] = {
        sizeof(Vec3)
    };
    for (UInt32 i = 0; i < AttributeCount; i++)
    {
        AttributeDescriptions[i].binding = 0;
        AttributeDescriptions[i].location = i;
        AttributeDescriptions[i].format = Formats[i];
        AttributeDescriptions[i].offset = Offset;
        Offset += Sizes[i];
    }

    // TODO: Descriptor set Layout

    VkPipelineShaderStageCreateInfo StageCreateInfos[OBJECT_SHADER_STAGE_COUNT];
    FMZeroMemory(StageCreateInfos, sizeof(StageCreateInfos));
    for (UInt32 i = 0; i < OBJECT_SHADER_STAGE_COUNT; i++)
    {
        StageCreateInfos[i].sType = Shader->Stages[i].ShaderStageCreateInfo.sType;
        StageCreateInfos[i] = Shader->Stages[i].ShaderStageCreateInfo;
    }

    if (!VulkanGraphicsPipelineCreate(
        Context,
        &Context->MainRenderpass,
        AttributeCount,
        AttributeDescriptions,
        0,
        0,
        OBJECT_SHADER_STAGE_COUNT,
        StageCreateInfos,
        Viewport,
        Scissor,
        false,
        &Shader->Pipeline
    ))
    {
        FLERROR("Failed to load the Graphics Pipeline for the Builtin Object shaders!");
        return false;
    }

    return true;
}

void VulkanObjectShaderDestroy(VulkanContext* Context, struct VulkanObjectShader* Shader) 
{
    VulkanPipelineDestroy(Context, &Shader->Pipeline);
    // Destroy Shader Modules
    for (UInt32 i = 0; i < OBJECT_SHADER_STAGE_COUNT; i++)
    {
        vkDestroyShaderModule(Context->Device.LogicalDevice, Shader->Stages[i].Handle, Context->Allocator);
        Shader->Stages[i].Handle = 0;
    }
}

void VulkanObjectShaderUse(VulkanContext* Context, struct VulkanObjectShader* Shader) 
{
    (void)Context;
    (void)Shader;
}