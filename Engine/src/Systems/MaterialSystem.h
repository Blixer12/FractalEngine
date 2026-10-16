/**
 * @file MaterialSystem.h
 * @author Blixer12
 * @brief The material system is responsible for managing materials in the
 * engine, including reference counting and auto-unloading.
 * 
 * @version 1.0
 * @date 2026-10-11
 * 
 * @copyright Fractal Engine is Copyright (c) Blixer12 2026
 * 
 */

#pragma once

#include "Defines.h"

#include "Resources/ResourceDef.h"

/** @brief The configuration for the material system. */
typedef struct MaterialSystemConfig {
    /** @brief The maximum number of loaded materials. */
    UInt32 MaxMaterialCount;
} MaterialSystemConfig;

/** @brief The name of the default material. */
constexpr char DefaultMaterialName[] = "Default";

/**
 * @brief Initializes the material system.
 * Should be called twice; once to get the memory requirement (passing state=0), and a second
 * time passing an allocated block of memory to actually initialize the system.
 * 
 * @param MemoryRequirement A pointer to hold the memory requirement as it is calculated.
 * @param State A block of memory to hold the state or, if gathering the memory requirement, 0.
 * @param Config The configuration for this system.
 * @return True on success; otherwise false.
 */
Bool8 MaterialSystemInitialize(UInt64* MemoryRequirement, void* State, MaterialSystemConfig Config);

/**
 * @brief Shuts down the material system.
 * 
 * @param State The state block of memory.
 */
void MaterialSystemShutdown(void* State);

/**
 * @brief Attempts to acquire a material with the given name. If it has not yet been loaded,
 * this triggers it to load. If the material is not found, a pointer to the default material
 * is returned. If the material _is_ found and loaded, its reference counter is incremented.
 * 
 * @param Name The name of the material to find.
 * @return A pointer to the loaded material. Can be a pointer to the default material if not found.
 */
Material* MaterialSystemAcquire(const char* Name);

/**
 * @brief Attempts to acquire a material from the given configuration. If it has not yet been loaded,
 * this triggers it to load. If the material is not found, a pointer to the default material
 * is returned. If the material _is_ found and loaded, its reference counter is incremented.
 * 
 * @param Config The config of the material to load.
 * @return A pointer to the loaded material.
 */
Material* MaterialSystemAcquireFromConfig(MaterialConfig Config);

/**
 * @brief Releases a material with the given name. Ignores non-existant materials.
 * Decreases the reference counter by 1. If the reference counter reaches 0 and
 * AutoRelease was set to true, the material is unloaded, releasing internal resources.
 * 
 * @param Name The name of the material to unload.
 */
void MaterialSystemRelease(const char* Name);

/**
 * @brief Gets a pointer to the default material. Does not reference count.
 */
Material* MaterialSystemGetDefault();