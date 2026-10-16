/**
 * @file CrystalBackend.h
 * @author Blixer12
 * @brief Hosts creation and destruction methods for Crystal's backend.
 * 
 * @version 1.0
 * @date 2026-10-11
 * 
 * @copyright Fractal Engine is Copyright (c) Blixer12 2026
 * 
 */

#pragma once

#include "CrystalDef.inl"

struct PlatformState;

/**
 * @brief Creates a new renderer backend of the given type.
 * 
 * @param Type The type of backend to create (e.g. Vulkan, OpenGL, DirectX)
 * @param Backend A pointer to hold the newly-created renderer backend.
 * @return True if successful; otherwise false.
 */
Bool8 CrystalBackendCreate(CrystalBackendType Type, CrystalBackend* Backend);

/**
 * @brief Destroys the given renderer backend.
 * 
 * @param Backend A pointer to the backend to be destroyed.
 */
void CrystalBackendDestroy(CrystalBackend* Backend);