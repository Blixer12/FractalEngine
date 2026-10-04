#include "LinearAllocator.h"

#include "Core/Memory.h"
#include "Core/Logger.h"

void LinearAllocatorCreate(UInt64 TotalSize, void* Memory, LinearAllocator* Allocator) {
    if (Allocator) {
        Allocator->TotalSize = TotalSize;
        Allocator->Allocated = 0;
        Allocator->OwnsMemory = Memory == 0;
        if (Memory) {
            Allocator->Memory = Memory;
        } else {
            Allocator->Memory = FMAllocate(TotalSize, MEMORY_TAG_LINEAR_ALLOCATOR);
        }
    }
}
void LinearAllocatorDestroy(LinearAllocator* Allocator) {
    if (Allocator) {
        Allocator->Allocated = 0;
        if (Allocator->OwnsMemory && Allocator->Memory) {
            FMFree(Allocator->Memory, Allocator->TotalSize, MEMORY_TAG_LINEAR_ALLOCATOR);
        } 
        Allocator->Memory = 0;
        Allocator->TotalSize = 0;
        Allocator->OwnsMemory = false;
    }
}

void* LinearAllocatorAllocate(LinearAllocator* Allocator, UInt64 Size) {
    if (Allocator && Allocator->Memory) {
        if (Allocator->Allocated + Size > Allocator->TotalSize) {
            UInt64 Remaining = Allocator->TotalSize - Allocator->Allocated;
            FLERROR("LinearAllocatorAllocate - Tried to allocate %lluB, only %lluB remaining.", Size, Remaining);
            return 0;
        }

        void* Block = ((UInt8*)Allocator->Memory) + Allocator->Allocated;
        Allocator->Allocated += Size;
        return Block;
    }

    FLERROR("LinearAllocatorAllocate - provided Allocator not initialized.");
    return 0;
}

void LinearAllocatorFreeAll(LinearAllocator* Allocator) {
    if (Allocator && Allocator->Memory) {
        Allocator->Allocated = 0;
        FMZeroMemory(Allocator->Memory, Allocator->TotalSize);
    }
}