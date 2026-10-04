#pragma once

#include "VulkanDef.inl"

Bool8 CreateShaderModule(
    VulkanContext* Context,
    const char* Name,
    const char* TypeString,
    VkShaderStageFlagBits ShaderStageFlags,
    UInt32 StageIndex,
    VulkanShaderStage* ShaderStages
);