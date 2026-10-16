/**
 * @file ResourceSystem.h
 * @author Blixer12
 * @brief This file contains the implementation of the resource system.
 * The resource system is responsible for managing resources and their
 * loaders in the engine.
 * 
 * @version 1.0
 * @date 2026-10-11
 * 
 * @copyright Fractal Engine is Copyright (c) Blixer12 2026
 * 
 */

#pragma once

#include "Resources/ResourceDef.h"

/** @brief The configuration for the resource system */
typedef struct ResourceSystemConfig {
     /** @brief The maximum number of loaders that can be registered with this system. */
    UInt32 MaxLoaderCount;
    /** @brief The relative base path for assets. */
    char* AssetBasePath;
} ResourceSystemConfig;

/** @brief An "interface" for a resource loader. All registered loaders use this. */
typedef struct ResourceLoader {
    /** @brief The loader identifier. */
    UInt32 ID;
    /** @brief The loader resource type. */
    ResourceType Type;
    /** @brief The loader custom type string, if type is set to custom. */
    const char* CustomType;
     /** @brief A type path which is prepended for the asset type. */
    const char* TypePath;

    /**
     * @brief Loads a resource using this loader.
     * @param Self A pointer to the loader itself.
     * @param Name The name of the resource to be loaded.
     * @param Resource A pointer to hold the loaded resource.
     * @returns True on success; otherwise false.
     */
    Bool8 (*Load)(struct ResourceLoader* Self, const char* Name, Resource* Resource);

    /**
     * @brief Unloads the given resource. Loader is determined by the resource's assigned loader id.
     * @param Self A pointer to the loader itself.
     * @param Name The name of the resource to be unloaded.
     */
    void (*Unload)(struct ResourceLoader* Self, Resource* Resource);
} ResourceLoader;

/**
 * @brief Initializes this system.
 * Should be called twice; once to get the memory requirement (passing state=0), and a second
 * time passing an allocated block of memory to actually initialize the system.
 * 
 * @param MemoryRequirement A pointer to hold the memory requirement as it is calculated.
 * @param State A block of memory to hold the state or, if gathering the memory requirement, 0.
 * @param Config The configuration for this system.
 * @return True on success; otherwise false.
 */
Bool8 ResourceSystemInitialize(UInt64* MemoryRequirement, void* State, ResourceSystemConfig Config);

/**
 * @brief Shuts down the resource system.
 * 
 * @param State The state block of memory for this system.
 */
void ResourceSystemShutdown(void* State);

/**
 * @brief Registers the given resource loader with the system.
 * 
 * @param Loader The loader to be registered.
 * @return True on success; otherwise false.
 */
FAPI Bool8 ResourceSystemRegisterLoader(ResourceLoader Loader);

/**
 * @brief Loads a resource of the given name.
 * 
 * @param Name The name of the resource to load.
 * @param Type The type of resource to load.
 * @param Resource A pointer to hold the newly-loaded resource.
 * @return True on success; otherwise false.
 */
FAPI Bool8 ResourceSystemLoad(const char* Name, ResourceType Type, Resource* Resource);

/**
 * @brief Loads a resource of the given name and of a custom type.
 * 
 * @param Name The name of the resource to load.
 * @param CustomType The custom resource type.
 * @param Resource A pointer to hold the newly-loaded resource.
 * @return True on success; otherwise false.
 */
FAPI Bool8 ResourceSystemLoadCustom(const char* Name, const char* CustomType, Resource* Resource);

/**
 * @brief Unloads the given resource.
 * 
 * @param Resource A pointer to the resource to be unloaded.
 */
FAPI void ResourceSystemUnload(Resource* Resource);

/** @brief Returns the base path of the resource system. */
FAPI const char* ResourceSystemBasePath();