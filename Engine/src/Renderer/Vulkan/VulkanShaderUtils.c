#include "VulkanShaderUtils.h"

#include "Core/FString.h"
#include "Core/Logger.h"
#include "Core/Memory.h"

#include "Systems/ResourceSystem.h"

Bool8 CreateShaderModule(
    VulkanContext* Context,
    const char* Name,
    const char* TypeString,
    VkShaderStageFlagBits ShaderStageFlags,
    UInt32 StageIndex,
    VulkanShaderStage* ShaderStages)
{
    // Build file name for the resource name
    char FileName[4096];
    StringFormat(FileName, "Shaders/%s.%s.spv", Name, TypeString);

    Resource BinaryResource;
    if (!ResourceSystemLoad(FileName, RESOURCE_TYPE_BINARY, &BinaryResource))
    {
        FLERROR("Unable to read shader module: '%s'", FileName);
        return false;
    }

    FMZeroMemory(&ShaderStages[StageIndex].CreateInfo, sizeof(VkShaderModuleCreateInfo));
    ShaderStages[StageIndex].CreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;

    ShaderStages[StageIndex].CreateInfo.codeSize = BinaryResource.DataSize;
    ShaderStages[StageIndex].CreateInfo.pCode    = (UInt32*)BinaryResource.Data;

    VK_CHECK(vkCreateShaderModule(
        Context->Device.LogicalDevice,
        &ShaderStages[StageIndex].CreateInfo,
        Context->Allocator,
        &ShaderStages[StageIndex].Handle));

    ResourceSystemUnload(&BinaryResource);

    // Shader stage info
    FMZeroMemory(&ShaderStages[StageIndex].ShaderStageCreateInfo, sizeof(VkPipelineShaderStageCreateInfo));
    ShaderStages[StageIndex].ShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    ShaderStages[StageIndex].ShaderStageCreateInfo.stage = ShaderStageFlags;
    ShaderStages[StageIndex].ShaderStageCreateInfo.module = ShaderStages[StageIndex].Handle;
    ShaderStages[StageIndex].ShaderStageCreateInfo.pName = "main";

    return true;
}