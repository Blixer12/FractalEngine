#pragma once

#include "Defines.h"

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

    MEMORY_TAG_MAX_TAGS
} MemoryTag;

FAPI void InitializeMemory();
FAPI void ShutdownMemory();

FAPI void* FMAllocate(UInt64 Size, MemoryTag Tag);
FAPI void FMFree(void* Block, UInt64 Size, MemoryTag Tag);
FAPI void* FMZeroMemory(void* Block, UInt64 Size);
FAPI void* FMCopyMemory(void* Dest, const void* Source, UInt64 Size);
FAPI void* FMSetMemory(void* Dest, Int32 Value, UInt64 Size);
FAPI char* FMGetMemoryUsageString();