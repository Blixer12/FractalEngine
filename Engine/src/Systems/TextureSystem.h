/**
 * @file TextureSystem.h
 * @author Blixer12
 * @brief This file contains the texture system, which handles the acquisition
 * and releasing of textures. It also reference monitors textures, and can
 * auto-release them when they no longer have any references, if configured to
 * 
 * @version 1.0
 * @date 2026-10-11
 * 
 * @copyright Fractal Engine is Copyright (c) Blixer12 2026
 * 
 */

#pragma once

#include "Renderer/CrystalDef.inl"

/** @brief The texture system configuration */
typedef struct TextureSystemConfig {
    /** @brief The maximum number of textures that can be loaded at once. */
    UInt32 MaxTextureCount;
} TextureSystemConfig;

/** @brief The default texture name. */
constexpr char DefaultTextureName[] = "Default";

/**
 * @brief Initializes the texture system.
 * Should be called twice; once to get the memory requirement (passing state=0), and a second
 * time passing an allocated block of memory to actually initialize the system.
 * 
 * @param MemoryRequirement A pointer to hold the memory requirement as it is calculated.
 * @param State A block of memory to hold the state or, if gathering the memory requirement, 0.
 * @param Config The configuration for this system.
 * @return True on success; otherwise false.
 */
Bool8 TextureSystemInitialize(UInt64* MemoryRequirement, void* State, TextureSystemConfig Config);

/**
 * @brief Shuts down the texture system.
 * 
 * @param State The state block of memory for this system.
 */
void TextureSystemShutdown(void* State);

/**
 * @brief Attempts to acquire a texture with the given name. If it has not yet been loaded,
 * this triggers it to load. If the texture is not found, a pointer to the default texture
 * is returned. If the texture _is_ found and loaded, its reference counter is incremented.
 * 
 * @param Name The name of the texture to find.
 * @param AutoRelease Indicates if the texture should auto-release when its reference count is 0.
 * Only takes effect the first time the texture is acquired.
 * @return A pointer to the loaded texture. Can be a pointer to the default texture if not found.
 */
Texture* TextureSystemAcquire(const char* Name, Bool8 AutoRelease);

/**
 * @brief Releases a texture with the given name. Ignores non-existant textures.
 * Decreases the reference counter by 1. If the reference counter reaches 0 and
 * AutoRelease was set to true, the texture is unloaded, releasing internal resources.
 * 
 * @param Name The name of the texture to unload.
 */
void TextureSystemRelease(const char* Name);

/**
 * @brief Gets a pointer to the default texture. No reference counting is 
 * done for default textures.
 */
Texture* TextureSystemGetDefault();