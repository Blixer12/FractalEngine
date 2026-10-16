/**
 * @file LoaderUtils.h
 * @author Blixer12
 * @brief This file contains resource loader utility/auxiliary functions.
 * 
 * @version 1.0
 * @date 2026-10-11
 * 
 * @copyright Fractal Engine is Copyright (c) Blixer12 2026
 * 
 */

#pragma once

#include "Defines.h"

#include "Core/Memory.h"

#include "Resources/ResourceDef.h"

struct ResourceLoader;

/**
 * @brief Unloads a resource using the appropriate registered loader.
 * 
 * @param Self A pointer to the resource loader to be used.
 * @param Resource A pointer to the resource to be unloaded.
 * @param Tag The memory tag that was used by the resource loader, and should be used to unload the resource.
 * @return True on success; otherwise false.
 */
Bool8 ResourceUnload(struct ResourceLoader* Self, Resource* Resource, MemoryTag Tag);
