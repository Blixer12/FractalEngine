/**
 * @file VulkanUIShader.h
 * @author Blixer12
 * @brief Contains the implementation of the Vulkan UI shader, which is used for
 * drawing most UI objects.
 * @version 1.0
 * @date 2026-10-11
 * 
 * @copyright Fractal Engine is Copyright (c) Blixer12 2026
 * 
 */

#pragma once

#include "Renderer/Vulkan/VulkanDef.inl"
#include "Renderer/CrystalDef.inl"

/**
 * @brief Creates a new Vulkan material shader.
 * 
 * @param Context A pointer to the Vulkan context.
 * @param Shader A pointer to hold the newly created shader.
 * @return True on success; otherwise false.
 */
Bool8 VulkanUIShaderCreate(VulkanContext* Context, VulkanUIShader* Shader);

/**
 * @brief Destroys the provided Vulkan material shader.
 * 
 * @param Context A pointer to the Vulkan context.
 * @param Shader A pointer to the shader to be destroyed.
 */
void VulkanUIShaderDestroy(VulkanContext* Context, struct VulkanUIShader* Shader);

/**
 * @brief "Uses" the shader, binding its internal pipeline.
 * 
 * @param Context A pointer to the Vulkan context.
 * @param Shader A pointer to the shader to be used.
 */
void VulkanUIShaderUse(VulkanContext* Context, struct VulkanUIShader* Shader);

/**
 * @brief Updates global properties of the shader, such as view and projection matrices.
 * 
 * @param Context A pointer to the Vulkan context.
 * @param Shader A pointer to the shader to be updated.
 * @param DeltaTime The time in seconds that has passed since the last frame.
 */
void VulkanUIShaderUpdateGlobalState(VulkanContext* Context, struct VulkanUIShader* Shader, Float32 DeltaTime);

/**
 * @brief Sets the model matrix on the shader.
 * 
 * @param Context A pointer to the Vulkan context.
 * @param Shader A pointer to the shader on which to set the matrix.
 * @param Model A copy of the model matrix to be set.
 */
void VulkanUIShaderSetModel(VulkanContext* Context, struct VulkanUIShader* Shader, Mat4 Model);

/**
 * @brief Applies various properties of the given material.
 * 
 * @param Context A pointer to the Vulkan context.
 * @param Shader A pointer to the shader on which to set the material properties.
 * @param Material A pointer to the material to be used.
 */
void VulkanUIShaderApplyMaterial(VulkanContext* Context, struct VulkanUIShader* Shader, Material* Material);

/**
 * @brief Acquires internal resources for the given material, such as descriptors.
 * 
 * @param Context A pointer to the Vulkan context.
 * @param Shader A pointer to the shader to acquire resources from.
 * @param Material A pointer to the material whose resources need to be acquired.
 * @return True on success; otherwise false.
 */
Bool8 VulkanUIShaderAcquireResources(VulkanContext* Context, struct VulkanUIShader* Shader, Material* Material);

/**
 * @brief Releases internal resources for the given material.
 * 
 * @param Context A pointer to the Vulkan context.
 * @param Shader A pointer to the shader to release resources from.
 * @param Material A pointer to the material whose resources need to be released.
 */
void VulkanUIShaderReleaseResources(VulkanContext* Context, struct VulkanUIShader* Shader, Material* Material);