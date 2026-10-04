#pragma once

#include "Defines.h"

typedef struct LinearAllocator {
    UInt64 TotalSize;
    UInt64 Allocated;
    void* Memory;
    Bool8 OwnsMemory;
} LinearAllocator;

FAPI void LinearAllocatorCreate(UInt64 TotalSize, void* Memory, LinearAllocator* Allocator);
FAPI void LinearAllocatorDestroy(LinearAllocator* Allocator);

FAPI void* LinearAllocatorAllocate(LinearAllocator* Allocator, UInt64 Size);
FAPI void LinearAllocatorFreeAll(LinearAllocator* Allocator);