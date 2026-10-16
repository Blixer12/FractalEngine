/**
 * @file Memory.h
 * @author Blixer12
 * @brief This file contains the structures and functions of the memory system.
 * This is responsible for memory interaction with the platform layer, such as
 * allocations/frees and tagging of memory allocations.
 * @note Note that reliance on this will likely be by core systems only, as items using
 * allocations directly will use allocators as they are added to the system.
 * 
 * @version 1.0
 * @date 2026-10-11
 * 
 * @copyright Fractal Engine is Copyright (c) Blixer12 2026
 * 
 */

#pragma once

#include "Defines.h"

/** @brief Tags to indicate the usage of memory allocations made in this system. */
typedef enum MemoryTag {
    // For temporary use. Should be assigned one of the below or have a new tag created.
    MEMORY_TAG_UNKNOWN,

    MEMORY_TAG_ARRAY,
    MEMORY_TAG_VECTOR,
    MEMORY_TAG_DICT,
    MEMORY_TAG_RING_QUEUE,
    MEMORY_TAG_BST,
    MEMORY_TAG_STRING,
    MEMORY_TAG_APPLICATION,
    MEMORY_TAG_JOB,
    MEMORY_TAG_TEXTURE,
    MEMORY_TAG_MATERIAL_INSTANCE,
    MEMORY_TAG_RENDERER,
    MEMORY_TAG_GAME,
    MEMORY_TAG_TRANSFORM,
    MEMORY_TAG_ENTITY,
    MEMORY_TAG_ENTITY_NODE,
    MEMORY_TAG_SCENE,

    // Allocators
    MEMORY_TAG_LINEAR_ALLOCATOR,

    MEMORY_TAG_MAX_TAGS
} MemoryTag;

/** @brief The configuration for the memory system. */
typedef struct MemorySystemConfig {
    /** @brief The total memory size in byes used by the internal allocator for this system. */
    UInt64 TotalAllocationSize;
} MemorySystemConfig;

/**
 * @brief Initializes the memory system
 * @param Config The configuration for the system
 */
FAPI Bool8 MemorySystemInitialize(MemorySystemConfig Config);

/**
 * @brief Shuts down the memory system.
 */
FAPI void MemorySystemShutdown();

/**
 * @brief Performs a memory allocation from the host of the given size. The allocation
 * is tracked for the provided tag.
 * @param Size The size of the allocation.
 * @param Tag Indicates the use of the allocated block.
 * @returns If successful, a pointer to a block of allocated memory; otherwise 0.
 */
FAPI void* FMAllocate(UInt64 Size, MemoryTag Tag);

/**
 * @brief Frees the given block, and untracks its size from the given tag.
 * @param Block A pointer to the block of memory to be freed.
 * @param Size The size of the block to be freed.
 * @param Tag The tag indicating the block's use.
 */
FAPI void FMFree(void* Block, UInt64 Size, MemoryTag Tag);

/**
 * @brief Zeroes out the provided memory block.
 * @param Block A pointer to the block of memory to be zeroed out.
 * @param Size The size in bytes to zero out.
 * @returns A pointer to the zeroed out block of memory.
 */
FAPI void* FMZeroMemory(void* Block, UInt64 Size);

/**
 * @brief Performs a copy of the memory at source to dest of the given size.
 * @param Dest A pointer to the destination block of memory to copy to.
 * @param Source A pointer to the source block of memory to copy from.
 * @param Size The amount of memory in bytes to be copied over.
 * @returns A pointer to the block of memory copied to.
 */
FAPI void* FMCopyMemory(void* Dest, const void* Source, UInt64 Size);

/**
 * @brief Sets the bytes of memory located at dest to value over the given size.
 * @param Dest A pointer to the destination block of memory to be set.
 * @param Value The value to be set.
 * @param Size The size in bytes to copy over to.
 * @returns A pointer to the destination block of memory.
 */
FAPI void* FMSetMemory(void* Dest, Int32 Value, UInt64 Size);

/**
 * @brief Obtains a string containing a "printout" of memory usage, categorized by
 * memory tag. The memory should be freed by the caller.
 * @deprecated This function should be discontinued in favour of something more robust in the future.
 * @returns A pointer to a character array containing the string representation of memory usage.
 */
FAPI char* FMGetMemoryUsageString();

/**
 * @brief Obtains the number of times kallocate was called since the memory system was initialized.
 * @returns The total count of allocations since the system's initialization.
 */
FAPI UInt64 GetMemoryAllocationCount();