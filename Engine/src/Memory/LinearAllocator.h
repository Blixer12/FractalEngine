/**
 * @file LinearAllocator.h
 * @author Blixer12
 * @brief This file contains the linear allocator implementation.
 * @details A linear allocator allocates memory from its internal block of memory
 * in a linear fashion. That is, one after another, moving a pointer along as it goes.
 * A linear allocator can perform allocations of any size, but allocation sizes are
 * not stored, and thus allocations made in this way are not individually freeable.
 * Only the entire thing can be freed. This comes with the benefit of speed at a cost
 * of flexibility.
 * 
 * @version 1.0
 * @date 2026-10-11
 * 
 * @copyright Fractal Engine is Copyright (c) Blixer12 2026
 * 
 */

#pragma once

#include "Defines.h"

/**
 * @brief The data structure for a linear allocator. 
 */
typedef struct LinearAllocator {
    /** @brief The total size of memory in the allocator. */
    UInt64 TotalSize;
    /** @brief The amount of memory currently allocated. */
    UInt64 Allocated;
    /** @brief The internal block of memory used by the allocator. */
    void* Memory;
    /** 
     * @brief Indicates if the allocator owns the memory (meaning it 
     * performed the allocation itself) or whether it was provided by an outside source.
     */
    Bool8 OwnsMemory;
} LinearAllocator;

/**
 * @brief Creates a linear allocator of the given size.
 * 
 * @param TotalSize The total amount in bytes the allocator will hold.
 * @param Memory Allocated block of memory matching size above, or 0. If 0, a dynamic allocation is performed
 * and this allocator is considered to own that memory.
 * @param Allocator A pointer to hold the new allocator.
 */
FAPI void LinearAllocatorCreate(UInt64 TotalSize, void* Memory, LinearAllocator* Allocator);

/**
 * @brief Destroys the given allocator. If the allocator owns its memory, it is freed at this time.
 * 
 * @param Allocator A pointer to the allocator to be destroyed.
 */
FAPI void LinearAllocatorDestroy(LinearAllocator* Allocator);


/**
 * @brief Allocates the given amount from the allocator.
 * 
 * @param Allocator A pointer to the allocator to allocate from.
 * @param Size The size to be allocated.
 * @return A pointer to a block of memory as allocated. If this fails, 0 is returned.
 */
FAPI void* LinearAllocatorAllocate(LinearAllocator* Allocator, UInt64 Size);

/**
 * @brief Frees everything in the allocator, effectively moving its pointer back to the beginning.
 * Does not free internal memory, if owned. Only resets the pointer.
 * 
 * @param Allocator A pointer to the allocator to free.
 */
FAPI void LinearAllocatorFreeAll(LinearAllocator* Allocator);