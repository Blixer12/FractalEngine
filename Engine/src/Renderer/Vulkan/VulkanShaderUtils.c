#include "VulkanShaderUtils.h"

#include "Core/FString.h"
#include "Core/Logger.h"
#include "Core/Memory.h"

#include "Platform/Filesystem.h"

Bool8 CreateShaderModule(
    VulkanContext* Context,
    const char* Name,
    const char* TypeString,
    VkShaderStageFlagBits ShaderStageFlags,
    UInt32 StageIndex,
    VulkanShaderStage* ShaderStages)
{
    // Build file name.
    char FileName[4096];
    StringFormat(FileName, "Assets/Shaders/%s.%s.spv", Name, TypeString);

    FMZeroMemory(&ShaderStages[StageIndex].CreateInfo, sizeof(VkShaderModuleCreateInfo));
    ShaderStages[StageIndex].CreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;

    // Obtain file handle.
    FileHandle Handle;
    if (!FilesystemOpen(FileName, FILE_MODE_READ, true, &Handle)) {
        FLERROR("Unable to read shader module: %s.", FileName);
        return false;
    }

    // Read the entire file as binary.
    UInt64 Size = 0;
    UInt8* FileBuffer = 0;
    if (!FilesystemReadAllBytes(&Handle, &FileBuffer, &Size)) {
        FLERROR("Unable to binary read shader module: %s.", FileName);
        return false;
    }
    ShaderStages[StageIndex].CreateInfo.codeSize = Size;
    ShaderStages[StageIndex].CreateInfo.pCode = (UInt32*)FileBuffer;

    // Close the file.
    FilesystemClose(&Handle);

    VK_CHECK(vkCreateShaderModule(
        Context->Device.LogicalDevice,
        &ShaderStages[StageIndex].CreateInfo,
        Context->Allocator,
        &ShaderStages[StageIndex].Handle));

    // Shader stage info
    FMZeroMemory(&ShaderStages[StageIndex].ShaderStageCreateInfo, sizeof(VkPipelineShaderStageCreateInfo));
    ShaderStages[StageIndex].ShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    ShaderStages[StageIndex].ShaderStageCreateInfo.stage = ShaderStageFlags;
    ShaderStages[StageIndex].ShaderStageCreateInfo.module = ShaderStages[StageIndex].Handle;
    ShaderStages[StageIndex].ShaderStageCreateInfo.pName = "main";

    if (FileBuffer) {
        FMFree(FileBuffer, sizeof(UInt8) * Size, MEMORY_TAG_STRING);
        FileBuffer = 0;
    }

    return true;
}