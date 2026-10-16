/**
 * @file VulkanMaterialShader.h
 * @author Blixer12
 * @brief This file contains the implementation of the Vulkan material shader,
 * used for rendering objects in the game world.D
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
Bool8 VulkanMaterialShaderCreate(VulkanContext* Context, VulkanMaterialShader* Shader);

/**
 * @brief Destroys the provided Vulkan material shader.
 * 
 * @param Context A pointer to the Vulkan context.
 * @param Shader A pointer to the shader to be destroyed.
 */
void VulkanMaterialShaderDestroy(VulkanContext* Context, struct VulkanMaterialShader* Shader);

/**
 * @brief "Uses" the shader, binding its internal pipeline.
 * 
 * @param Context A pointer to the Vulkan context.
 * @param Shader A pointer to the shader to be used.
 */
void VulkanMaterialShaderUse(VulkanContext* Context, struct VulkanMaterialShader* Shader);

/**
 * @brief Updates global properties of the shader, such as view and projection matrices.
 * 
 * @param Context A pointer to the Vulkan context.
 * @param Shader A pointer to the shader to be updated.
 * @param DeltaTime The time in seconds that has passed since the last frame.
 */
void VulkanMaterialShaderUpdateGlobalState(VulkanContext* Context, struct VulkanMaterialShader* Shader, Float32 DeltaTime);

/**
 * @brief Sets the model matrix on the shader.
 * 
 * @param Context A pointer to the Vulkan context.
 * @param Shader A pointer to the shader on which to set the matrix.
 * @param Model A copy of the model matrix to be set.
 */
void VulkanMaterialShaderSetModel(VulkanContext* Context, struct VulkanMaterialShader* Shader, Mat4 Model);

/**
 * @brief Applies various properties of the given material.
 * 
 * @param Context A pointer to the Vulkan context.
 * @param Shader A pointer to the shader on which to set the material properties.
 * @param Material A pointer to the material to be used.
 */
void VulkanMaterialShaderApplyMaterial(VulkanContext* Context, struct VulkanMaterialShader* Shader, Material* Material);

/**
 * @brief Acquires internal resources for the given material, such as descriptors.
 * 
 * @param Context A pointer to the Vulkan context.
 * @param Shader A pointer to the shader to acquire resources from.
 * @param Material A pointer to the material whose resources need to be acquired.
 * @return True on success; otherwise false.
 */
Bool8 VulkanMaterialShaderAcquireResources(VulkanContext* Context, struct VulkanMaterialShader* Shader, Material* Material);

/**
 * @brief Releases internal resources for the given material.
 * 
 * @param Context A pointer to the Vulkan context.
 * @param Shader A pointer to the shader to release resources from.
 * @param Material A pointer to the material whose resources need to be released.
 */
void VulkanMaterialShaderReleaseResources(VulkanContext* Context, struct VulkanMaterialShader* Shader, Material* Material);