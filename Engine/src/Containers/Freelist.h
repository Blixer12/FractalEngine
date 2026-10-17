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
    /** @brief The internal State of the free list. */
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
 * @brief Attempts to resize the provided freelist to the given size. Internal data is copied to the new
 * block of memory. The old block must be freed after this call.
 * NOTE: New size must be _greater_ than the existing size of the given list.
 * NOTE: Should be called twice; once to query the memory requirement (passing NewMemory=0),
 * and a second time to actually resize the list.
 * 
 * @param List A pointer to the list to be resized.
 * @param MemoryRequirement A pointer to hold the amount of memory required for the resize.
 * @param NewMemory The new block of State memory. Set to 0 if only querying MemoryRequirement.
 * @param NewSize The new size. Must be greater than the size of the provided list.
 * @param OldMemory A pointer to hold the old block of memory so that it may be freed after this call.
 * @return True on success; otherwise false.
 */
FAPI Bool8 FreelistResize(Freelist* List, UInt64* MemoryRequirement, void* NewMemory, UInt64 NewSize, void** OldMemory);

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