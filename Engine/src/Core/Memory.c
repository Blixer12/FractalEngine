#include "Memory.h"

#include "Logger.h"
#include "Platform/Platform.h"
#include "FString.h"
#include "Memory/DynamicAllocator.h"

#include <stdio.h>

struct MemoryStats {
    UInt64 TotalAllocated;
    UInt64 TaggedAllocations[MEMORY_TAG_MAX_TAGS];
};

static const char* MemoryTagStrings[MEMORY_TAG_MAX_TAGS] = {
    "UNKNOWN         ",
    "ARRAY           ",
    "VECTOR          ",
    "DICT            ",
    "RING_QUEUE      ",
    "BST             ",
    "STRING          ",
    "APPLICATION     ",
    "JOB             ",
    "TEXTURE         ",
    "MAT_INST        ",
    "RENDERER        ",
    "GAME            ",
    "TRANSFORM       ",
    "ENTITY          ",
    "ENTITY_NODE     ",
    "SCENE           ",
    "LINEAR_ALLOCATOR"};

typedef struct MemoryState {
    MemorySystemConfig Config;
    struct MemoryStats Stats;
    UInt64 AllocatorMemoryRequirements;
    DynamicAllocator Allocator;
    void* AllocatorBlock;
    UInt64 AllocationCount;
} MemoryState;

static MemoryState* StatePtr;

Bool8 MemorySystemInitialize(MemorySystemConfig Config)
{
    UInt64 StateMemoryRequirement = sizeof(MemoryState);

    UInt64 AllocationRequirement = 0;
    DynamicAllocatorCreate(Config.TotalAllocationSize, &AllocationRequirement, 0, 0);

    void* Block = PlatformAllocate(StateMemoryRequirement + AllocationRequirement, false);
    if (!Block)
    {
        FLFATAL("Memory system alloc failed!");
        return false;
    }

    StatePtr = (MemoryState*)Block;
    StatePtr->Config = Config;
    StatePtr->AllocationCount = 0;
    StatePtr->AllocatorMemoryRequirements = AllocationRequirement;

    PlatformZeroMemory(&StatePtr->Stats, sizeof(StatePtr->Stats));

    StatePtr->AllocatorBlock = ((void*)Block + StateMemoryRequirement);

    if (!DynamicAllocatorCreate(Config.TotalAllocationSize, &StatePtr->AllocatorMemoryRequirements, StatePtr->AllocatorBlock, &StatePtr->Allocator))
    {
        FLFATAL("Memory system is unable to set up internal allocator");
        return false;
    }

    FLDEBUG("Memory system sucessfully allocated %llu bytes.", Config.TotalAllocationSize);
    return true;
}

void MemorySystemShutdown()
{
    if (StatePtr)
    {
        DynamicAllocatorDestroy(&StatePtr->Allocator);
        PlatformFree(StatePtr, StatePtr->AllocatorMemoryRequirements + sizeof(MemoryState));
    }
    StatePtr = 0;
}

void* FMAllocate(UInt64 Size, MemoryTag Tag)
{
    if (Tag == MEMORY_TAG_UNKNOWN)
    {
        #if FRELEASE == 1
        FLFATAL("FMAllocate called with an untracked tag (MEMORY_TAG_UNKNOWN). In a Release build, Untracked allocations are strictly forbidden.");
        #else
        FLWARN("FMAllocate called with an untracked tag (MEMORY_TAG_UNKNOWN). We Recommend to Re-tag this allocation");
        #endif
    }

    void * Block = 0;
    if (StatePtr)
    {
        StatePtr->Stats.TotalAllocated += Size;
        StatePtr->Stats.TaggedAllocations[Tag] += Size;
        StatePtr->AllocationCount++;

        Block = DynamicAllocatorAllocate(&StatePtr->Allocator, Size);
    } else {
        FLWARN("FMAllocate called befire system init!");
        Block = PlatformAllocate(Size, false);
    }
    
    //TODO: Memory Allignment

    if (Block)
    {
        PlatformZeroMemory(Block, Size);
        return Block;
    }
    FLFATAL("FMAllocate failed to allocate successfully!");
    return nullptr;
}

void FMFree(void* Block, UInt64 Size, MemoryTag Tag)
{
        if (Tag == MEMORY_TAG_UNKNOWN)
    {
        #if FRELEASE == 1
        FLFATAL("FMAllocate called with an untracked tag (MEMORY_TAG_UNKNOWN). In a Release build, Untracked allocations are strictly forbidden.");
        #else
        FLWARN("FMAllocate called with an untracked tag (MEMORY_TAG_UNKNOWN). We Recommend to Re-tag this allocation");
        #endif
    }

    //TODO: Memory Allignment
    if (StatePtr)
    {
        StatePtr->Stats.TotalAllocated -= Size;
        StatePtr->Stats.TaggedAllocations[Tag] -= Size;
        Bool8 Result = DynamicAllocatorFree(&StatePtr->Allocator, Block, Size);

        if (!Result)
        {
            PlatformFree(Block, false);
        } 
    } else {
        PlatformFree(Block, false);
    }

}

void* FMZeroMemory(void* Block, UInt64 Size) 
{
    return PlatformZeroMemory(Block, Size);
}

void* FMCopyMemory(void* Dest, const void* Source, UInt64 Size) 
{
    return PlatformCopyMemory(Dest, Source, Size);
}

void* FMSetMemory(void* Dest, Int32 Value, UInt64 Size) 
{
    return PlatformSetMemory(Dest, Value, Size);
}

char* FMGetMemoryUsageString()
{
    const UInt64 GB = 1024 * 1024 * 1024;
    const UInt64 MB = 1024 * 1024;
    const UInt64 KB = 1024;

    char Buffer[32768] = "System memory use (Tagged):\n";
    UInt64 Offset = StringLength(Buffer);
    for(UInt32 i = 0; i < MEMORY_TAG_MAX_TAGS; ++i)
    {
        char unit[6] = "Bytes";
        Float64 amount = 1.0;

        if (StatePtr->Stats.TaggedAllocations[i] >= GB) {
            unit[0] = 'G';
            unit[1] = 'B';
            unit[2] = 0;   // Force the string to end right here, cutting off any leftover letters!
            amount = StatePtr->Stats.TaggedAllocations[i] / (Float64)GB;
        } else if (StatePtr->Stats.TaggedAllocations[i] >= MB) {
            unit[0] = 'M';
            unit[1] = 'B';
            unit[2] = 0;   // Force the string to end here
            amount = StatePtr->Stats.TaggedAllocations[i] / (Float64)MB;
        } else if (StatePtr->Stats.TaggedAllocations[i] >= KB) {
            unit[0] = 'K';
            unit[1] = 'B';
            unit[2] = 0;   // Force the string to end here
            amount = StatePtr->Stats.TaggedAllocations[i] / (Float64)KB;
        } else {
            unit[0] = 'B';
            unit[1] = 'y';
            unit[2] = 't';
            unit[3] = 'e';
            unit[4] = 's';
            unit[5] = 0;   // Properly null-terminate the full word
            amount = (Float64)StatePtr->Stats.TaggedAllocations[i];
        }

        Int32 Length = snprintf(Buffer + Offset, 32768, "  %s: %.2f %s\n",MemoryTagStrings[i], amount, unit);
        Offset += Length;
    }

    char* FinalString = StringDuplicate(Buffer);    
    return FinalString;
}

UInt64 GetMemoryAllocationCount()
{
    if (StatePtr)
    {
        return StatePtr->AllocationCount;
    }
    return 0;
}