#pragma once

#include "Renderer/Vulkan/VulkanDef.inl"
#include "Renderer/CrystalDef.inl"

Bool8 VulkanObjectShaderCreate(VulkanContext* Context, VulkanObjectShader* Shader);

void VulkanObjectShaderDestroy(VulkanContext* Context, struct VulkanObjectShader* Shader);

void VulkanObjectShaderUse(VulkanContext* Context, struct VulkanObjectShader* Shader);

void VulkanObjectShaderUpdateGlobalState(VulkanContext* Context, struct VulkanObjectShader* Shader);

void VulkanObjectShaderUpdateObject(VulkanContext* Context, struct VulkanObjectShader* Shader, Mat4 Model);