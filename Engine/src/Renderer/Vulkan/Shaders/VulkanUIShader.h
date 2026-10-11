#pragma once

#include "Renderer/Vulkan/VulkanDef.inl"
#include "Renderer/CrystalDef.inl"

Bool8 VulkanUIShaderCreate(VulkanContext* Context, VulkanUIShader* Shader);

void VulkanUIShaderDestroy(VulkanContext* Context, struct VulkanUIShader* Shader);

void VulkanUIShaderUse(VulkanContext* Context, struct VulkanUIShader* Shader);

void VulkanUIShaderUpdateGlobalState(VulkanContext* Context, struct VulkanUIShader* Shader, Float32 DeltaTime);

void VulkanUIShaderSetModel(VulkanContext* Context, struct VulkanUIShader* Shader, Mat4 Model);
void VulkanUIShaderApplyMaterial(VulkanContext* Context, struct VulkanUIShader* Shader, Material* Material);

Bool8 VulkanUIShaderAcquireResources(VulkanContext* Context, struct VulkanUIShader* Shader, Material* Material);
void VulkanUIShaderReleaseResources(VulkanContext* Context, struct VulkanUIShader* Shader, Material* Material);