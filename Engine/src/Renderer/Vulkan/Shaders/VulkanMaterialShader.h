#pragma once

#include "Renderer/Vulkan/VulkanDef.inl"
#include "Renderer/CrystalDef.inl"

Bool8 VulkanMaterialShaderCreate(VulkanContext* Context, VulkanMaterialShader* Shader);

void VulkanMaterialShaderDestroy(VulkanContext* Context, struct VulkanMaterialShader* Shader);

void VulkanMaterialShaderUse(VulkanContext* Context, struct VulkanMaterialShader* Shader);

void VulkanMaterialShaderUpdateGlobalState(VulkanContext* Context, struct VulkanMaterialShader* Shader, Float32 DeltaTime);

void VulkanMaterialShaderUpdateObject(VulkanContext* Context, struct VulkanMaterialShader* Shader, GeometryRenderData Data);

Bool8 VulkanMaterialShaderAcquireResources(VulkanContext* Context, struct VulkanMaterialShader* Shader, Material* Material);
void VulkanMaterialShaderReleaseResources(VulkanContext* Context, struct VulkanMaterialShader* Shader, Material* Material);