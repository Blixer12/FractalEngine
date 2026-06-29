#include "Memory.h"

#include "Logger.h"
#include "Platform/Platform.h"
#include "FString.h"

#include <stdio.h>

struct MemoryStats {
    UInt64 TotalAllocated;
    UInt64 TaggedAllocations[MEMORY_TAG_MAX_TAGS];
};

static const char* MemoryTagStrings[MEMORY_TAG_MAX_TAGS] = {
    "UNKNOWN    ",
    "ARRAY      ",
    "VECTOR     ",
    "DICT       ",
    "RING_QUEUE ",
    "BST        ",
    "STRING     ",
    "APPLICATION",
    "JOB        ",
    "TEXTURE    ",
    "MAT_INST   ",
    "RENDERER   ",
    "GAME       ",
    "TRANSFORM  ",
    "ENTITY     ",
    "ENTITY_NODE",
    "SCENE      "};

static struct MemoryStats Stats;

void InitializeMemory()
{
    PlatformZeroMemory(&Stats, sizeof(Stats));
}

void ShutdownMemory()
{

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

    Stats.TotalAllocated += Size;
    Stats.TaggedAllocations[Tag] += Size;

    //TODO: Memory Allignment
    void* Block = PlatformAllocate(Size, false);
    PlatformZeroMemory(Block, Size);
    return Block;
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
    Stats.TotalAllocated -= Size;
    Stats.TaggedAllocations[Tag] -= Size;
    PlatformFree(Block, false);
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
    for(UInt32 i = 0; i < MEMORY_TAG_MAX_TAGS; i++)
    {
        char unit[6] = "Bytes";
        Float64 amount = 1.0;

        if (Stats.TaggedAllocations[i] >= GB) {
            unit[0] = 'G';
            unit[1] = 'B';
            unit[2] = 0;   // Force the string to end right here, cutting off any leftover letters!
            amount = Stats.TaggedAllocations[i] / (Float64)GB;
        } else if (Stats.TaggedAllocations[i] >= MB) {
            unit[0] = 'M';
            unit[1] = 'B';
            unit[2] = 0;   // Force the string to end here
            amount = Stats.TaggedAllocations[i] / (Float64)MB;
        } else if (Stats.TaggedAllocations[i] >= KB) {
            unit[0] = 'K';
            unit[1] = 'B';
            unit[2] = 0;   // Force the string to end here
            amount = Stats.TaggedAllocations[i] / (Float64)KB;
        } else {
            unit[0] = 'B';
            unit[1] = 'y';
            unit[2] = 't';
            unit[3] = 'e';
            unit[4] = 's';
            unit[5] = 0;   // Properly null-terminate the full word
            amount = (Float64)Stats.TaggedAllocations[i];
        }

        Int32 Length = snprintf(Buffer + Offset, 32768, "  %s: %.2f %s\n",MemoryTagStrings[i], amount, unit);
        Offset += Length;
    }

    char* FinalString = StringDuplicate(Buffer);    
    return FinalString;
}