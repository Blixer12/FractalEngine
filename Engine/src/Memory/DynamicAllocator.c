#include "DynamicAllocator.h"

#include "Core/Memory.h"
#include "Core/Logger.h"
#include "Containers/Freelist.h"

typedef struct DynamicAllocatorState {
    UInt64 TotalSize;
    Freelist List;
    void* FreelistBlock;
    void* MemoryBlock;
} DynamicAllocatorState;

Bool8 DynamicAllocatorCreate(UInt64 TotalSize, UInt64* MemoryRequirement, void* Memory, DynamicAllocator* OutAllocator) {
    if (TotalSize < 1) {
        FLERROR("DynamicAllocatorCreate cannot have a TotalSize of 0. Create failed.");
        return false;
    }
    if (!MemoryRequirement) {
        FLERROR("DynamicAllocatorCreate requires MemoryRequirement to exist. Create failed.");
        return false;
    }
    UInt64 FreelistRequirement = 0;
    // Grab the memory requirement for the free list first.
    FreelistCreate(TotalSize, &FreelistRequirement, 0, 0);

    *MemoryRequirement = FreelistRequirement + sizeof(DynamicAllocatorState) + TotalSize;

    // If only obtaining requirement, boot out.
    if (!Memory) {
        return true;
    }

    // Memory layout:
    // state
    // freelist block
    // memory block
    OutAllocator->Memory = Memory;
    DynamicAllocatorState* State = OutAllocator->Memory;
    State->TotalSize = TotalSize;
    State->FreelistBlock = (void*)(OutAllocator->Memory + sizeof(DynamicAllocatorState));
    State->MemoryBlock = (void*)(State->FreelistBlock + FreelistRequirement);

    // Actually create the freelist
    FreelistCreate(TotalSize, &FreelistRequirement, State->FreelistBlock, &State->List);

    FMZeroMemory(State->MemoryBlock, TotalSize);
    return true;
}

Bool8 DynamicAllocatorDestroy(DynamicAllocator* Allocator) {
    if (Allocator) {
        DynamicAllocatorState* State = Allocator->Memory;
        FreelistDestroy(&State->List);
        FMZeroMemory(State->MemoryBlock, State->TotalSize);
        State->TotalSize = 0;
        Allocator->Memory = 0;
        return true;
    }

    FLWARN("DynamicAllocatorDestroy requires a pointer to an allocator. Destroy failed.");
    return false;
}

void* DynamicAllocatorAllocate(DynamicAllocator* Allocator, UInt64 Size) {
    if (Allocator && Size) {
        DynamicAllocatorState* State = Allocator->Memory;
        UInt64 Offset = 0;
        // Attempt to allocate from the freelist.
        if (FreelistAllocateBlock(&State->List, Size, &Offset)) {
            // Use that offset against the base memory block to get the block.
            void* Block = (void*)(State->MemoryBlock + Offset);
            return Block;
        } else {
            FLERROR("DynamicAllocatorAllocate no blocks of memory large enough to allocate from.");
            UInt64 Available = FreelistFreeSpace(&State->List);
            FLERROR("Requested size: %llu, total space available: %llu", Size, Available);
            // TODO: Report fragmentation?
            return 0;
        }
    }

    FLERROR("DynamicAllocatorAllocate requires a valid allocator and size.");
    return 0;
}

Bool8 DynamicAllocatorFree(DynamicAllocator* Allocator, void* Block, UInt64 Size) {
    if (!Allocator || !Block) {
        FLERROR("DynamicAllocatorFree requires both a valid allocator (0x%p) and a block (0x%p) to be freed.", Allocator, Block);
        return false;
    }

    DynamicAllocatorState* State = Allocator->Memory;
    if (Block < State->MemoryBlock || Block > State->MemoryBlock + State->TotalSize) {
        void* EndOfBlock = (void*)(State->MemoryBlock + State->TotalSize);
        FLERROR("DynamicAllocatorFree trying to release block (0x%p) outside of allocator range (0x%p)-(0x%p)", Block, State->MemoryBlock, EndOfBlock);
        return false;
    }
    UInt64 Offset = (Block - State->MemoryBlock);
    if (!FreelistFreeBlock(&State->List, Size, Offset)) {
        FLERROR("DynamicAllocatorFree failed.");
        return false;
    }

    return true;
}

UInt64 DynamicAllocatorFreeSpace(DynamicAllocator* Allocator) {
    DynamicAllocatorState* State = Allocator->Memory;
    return FreelistFreeSpace(&State->List);
}