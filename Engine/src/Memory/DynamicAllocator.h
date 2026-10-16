/**
 * @file DynamicAllocator.h
 * @author Blixer12
 * @brief Contains the dynamic allocator implementation.
 * @details A dynamic allocator allocates memory from its internal block 
 * in an unordered fashion, searching for the first available free node or a block that fits.
 * 
 * Unlike linear allocators, which are typically used for the entire lifetime of the 
 * application and only advance a pointer, dynamic allocators allow individual blocks 
 * of any size to be freed independently. This provides greater flexibility at the cost 
 * of speed, making dynamic allocators ideal for memory regions that are freed frequently.
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
 * @brief A dynamic memory allocator structure.
 */
typedef struct DynamicAllocator {
    void* Memory;
} DynamicAllocator;

/**
 * @brief Creates a new dynamic allocator or obtains its memory requirement.
 * 
 * @param TotalSize The total size in bytes for the allocator.
 * @param MemoryRequirement Pointer to hold the memory requirement.
 * @param Memory Pre-allocated memory block or 0 to query requirement.
 * @param OutAllocator Pointer to hold the created dynamic allocator.
 * @return Bool8 True if successful; otherwise false.
 */
FAPI Bool8 DynamicAllocatorCreate(UInt64 TotalSize, UInt64* MemoryRequirement, void* Memory, DynamicAllocator* OutAllocator);

/**
 * @brief Destroys the provided dynamic allocator.
 * 
 * @param Allocator Pointer to the allocator to destroy.
 * @return Bool8 True if successful; otherwise false.
 */
FAPI Bool8 DynamicAllocatorDestroy(DynamicAllocator* Allocator);

/**
 * @brief Allocates a block of memory of the given size from the dynamic allocator.
 * 
 * @param Allocator Pointer to the allocator.
 * @param Size The size in bytes to allocate.
 * @return void* Pointer to the allocated memory block, or 0 on failure.
 */
FAPI void* DynamicAllocatorAllocate(DynamicAllocator* Allocator, UInt64 Size);

/**
 * @brief Frees a previously allocated block of memory.
 * 
 * @param Allocator Pointer to the allocator.
 * @param Block Pointer to the memory block to free.
 * @param Size The size of the block in bytes.
 */
FAPI Bool8 DynamicAllocatorFree(DynamicAllocator* Allocator, void* Block, UInt64 Size);

/**
 * @brief Returns the amount of free space remaining in the allocator.
 * 
 * @param Allocator Pointer to the allocator.
 * @return UInt64 The amount of free space in bytes.
 */
FAPI UInt64 DynamicAllocatorFreeSpace(DynamicAllocator* Allocator);