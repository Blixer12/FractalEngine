/**
 * @file VulkanShaderUtils.h
 * @author Blixer12
 * @brief A collection of shader utility functions.
 * @version 1.0
 * @date 2026-10-11
 * 
 * @copyright Fractal Engine is Copyright (c) Blixer12 2026
 * 
 */

#pragma once

#include "VulkanDef.inl"

/**
 * @brief Create a shader module object
 * 
 * @param Context A pointer to the Vulkan context.
 * @param Name The name of the shader module.
 * @param TypeString A string representation of the shader type.
 * @param ShaderStageFlags The flag indicating the shader stage.
 * @param StageIndex The index of the shader stage.
 * @param ShaderStages An array of shader stages.
 * @return True on success; otherwise false.
 */
Bool8 CreateShaderModule(
    VulkanContext* Context,
    const char* Name,
    const char* TypeString,
    VkShaderStageFlagBits ShaderStageFlags,
    UInt32 StageIndex,
    VulkanShaderStage* ShaderStages
);