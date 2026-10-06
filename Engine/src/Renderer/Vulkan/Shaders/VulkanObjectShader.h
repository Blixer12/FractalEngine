#pragma once

#include "Renderer/Vulkan/VulkanDef.inl"
#include "Renderer/CrystalDef.inl"

Bool8 VulkanObjectShaderCreate(VulkanContext* Context, VulkanObjectShader* Shader);

void VulkanObjectShaderDestroy(VulkanContext* Context, struct VulkanObjectShader* Shader);

void VulkanObjectShaderUse(VulkanContext* Context, struct VulkanObjectShader* Shader);

void VulkanObjectShaderUpdateGlobalState(VulkanContext* Context, struct VulkanObjectShader* Shader, Float32 DeltaTime);

void VulkanObjectShaderUpdateObject(VulkanContext* Context, struct VulkanObjectShader* Shader, GeometryRenderData* Data);

Bool8 VulkanObjectShaderAcquireResources(VulkanContext* Context, struct VulkanObjectShader* Shader, UInt64* ObjectID);
void VulkanObjectShaderReleaseResources(VulkanContext* Context, struct VulkanObjectShader* Shader, UInt64 ObjectID);