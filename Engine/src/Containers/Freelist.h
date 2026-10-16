/**
 * @file Freelist.h
 * @author Blixer12
 * @brief Dynamic alloc memory management interface
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
 * @brief A data structure to be used alongside an allocator for dynamic memory
 * allocation. Tracks free ranges of memory.
 */
typedef struct Freelist {
    /** @brief The internal state of the free list. */
    void* Memory;
} Freelist;

/**
 * @brief Creates a new free list or obtains the memory requirement for one. Call
 * twice; once passing 0 to memory to obtain memory requirement, and a second
 * time passing an allocated block to memory.
 * 
 * @param TotalSize The total size in bytes that the free list should track.
 * @param MemoryRequirement A pointer to hold memory requirement for the free list itself.
 * @param Memory 0, or a pre-allocated block of memory for the free list to use.
 * @param OutList A pointer to hold the created free list.
 */
FAPI void FreelistCreate(UInt64 TotalSize, UInt64* MemoryRequirement, void* Memory, Freelist* OutList);

/**
 * @brief Destroys the provided list.
 * 
 * @param List The list to be destroyed.
 */
FAPI void FreelistDestroy(Freelist* List);

/**
 * @brief Attempts to find a free block of memory of the given size.
 * 
 * @param List A pointer to the list to search.
 * @param Size The size to allocate.
 * @param OutOffset A pointer to hold the offset to the allocated memory.
 * @return Bool8 True if a block of memory was found and allocated; otherwise false.
 */
FAPI Bool8 FreelistAllocateBlock(Freelist* List, UInt64 Size, UInt64* OutOffset);

/**
 * @brief Attempts to free a block of memory at the given offset, and of the given
 * size. Can fail if invalid data is passed.
 * 
 * @param List A pointer to the list to free from.
 * @param Size The size to be freed.
 * @param Offset The offset to free at.
 * @return Bool8 True if successful; otherwise false. False should be treated as an error.
 */
FAPI Bool8 FreelistFreeBlock(Freelist* List, UInt64 Size, UInt64 Offset);

/**
 * @brief Clears the free list.
 * 
 * @param List The list to be cleared.
 */
FAPI void FreelistClear(Freelist* List);

/**
 * @brief Returns the amount of free space in this list. NOTE: Since this has
 * to iterate the entire internal list, this can be an expensive operation.
 * Use sparingly.
 * 
 * @param List A pointer to the list to obtain from.
 * @return The amount of free space in bytes.
 */
FAPI UInt64 FreelistFreeSpace(Freelist* List);