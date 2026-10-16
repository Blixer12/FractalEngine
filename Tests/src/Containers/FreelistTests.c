#include "FreelistTests.h"
#include "../TestManager.h"
#include "../Expect.h"

#include <Defines.h>
#include <Containers/Freelist.h>
#include <Core/Memory.h>
#include <Core/Logger.h>

UInt8 FreelistShouldCreateAndDestroy() {
    // NOTE: creating a small size list, which will trigger a warning.
    FLDEBUG("The following warning message is intentional.");

    Freelist List;

    // Get the memory requirement
    UInt64 MemoryRequirement = 0;
    UInt64 TotalSize = 40;
    FreelistCreate(TotalSize, &MemoryRequirement, 0, 0);

    // Allocate and create the freelist.
    void* Block = FMAllocate(MemoryRequirement, MEMORY_TAG_APPLICATION);
    FreelistCreate(TotalSize, &MemoryRequirement, Block, &List);

    // Verify that the memory was assigned.
    ExpectShouldNotBe(0, List.Memory);
    // Verify that the entire block is free.
    UInt64 FreeSpace = FreelistFreeSpace(&List);
    ExpectShouldBe(TotalSize, FreeSpace);

    // Destroy and verify that the memory was unassigned.
    FreelistDestroy(&List);
    ExpectShouldBe(0, List.Memory);
    FMFree(Block, MemoryRequirement, MEMORY_TAG_APPLICATION);

    return true;
}

UInt8 FreelistShouldAllocateOneAndFreeOne() {
    Freelist List;

    // Get the memory requirement
    UInt64 MemoryRequirement = 0;
    UInt64 TotalSize = 512;
    FreelistCreate(TotalSize, &MemoryRequirement, 0, 0);

    // Allocate and create the freelist.
    void* Block = FMAllocate(MemoryRequirement, MEMORY_TAG_APPLICATION);
    FreelistCreate(TotalSize, &MemoryRequirement, Block, &List);

    // Allocate some space.
    UInt64 Offset = InvalidID;  // Start with invalid id, which is a good default since it should never happen.
    Bool8 Result = FreelistAllocateBlock(&List, 64, &Offset);
    // Verify that result is true, offset should be set to 0.
    ExpectToBeTrue(Result);
    ExpectShouldBe(0, Offset);

    // Verify that the correct amount of space is free.
    UInt64 FreeSpace = FreelistFreeSpace(&List);
    ExpectShouldBe(TotalSize - 64, FreeSpace);

    // Now free the block.
    Result = FreelistFreeBlock(&List, 64, Offset);
    // Verify that result is true
    ExpectToBeTrue(Result);

    // Verify the entire block is free.
    FreeSpace = FreelistFreeSpace(&List);
    ExpectShouldBe(TotalSize, FreeSpace);

    // Destroy and verify that the memory was unassigned.
    FreelistDestroy(&List);
    ExpectShouldBe(0, List.Memory);
    FMFree(Block, MemoryRequirement, MEMORY_TAG_APPLICATION);

    return true;
}

UInt8 FreelistShouldAllocateOneAndFreeMulti() {
    Freelist List;

    // Get the memory requirement
    UInt64 MemoryRequirement = 0;
    UInt64 TotalSize = 512;
    FreelistCreate(TotalSize, &MemoryRequirement, 0, 0);

    // Allocate and create the freelist.
    void* Block = FMAllocate(MemoryRequirement, MEMORY_TAG_APPLICATION);
    FreelistCreate(TotalSize, &MemoryRequirement, Block, &List);

    // Allocate some space.
    UInt64 Offset = InvalidID;  // Start with invalid id, which is a good default since it should never happen.
    Bool8 Result = FreelistAllocateBlock(&List, 64, &Offset);
    // Verify that result is true, offset should be set to 0.
    ExpectToBeTrue(Result);
    ExpectShouldBe(0, Offset);

    // Allocate some more space.
    UInt64 Offset2 = InvalidID;  // Start with invalid id, which is a good default since it should never happen.
    Result = FreelistAllocateBlock(&List, 64, &Offset2);
    // Verify that result is true, offset should be set to the size of the previous allocation.
    ExpectToBeTrue(Result);
    ExpectShouldBe(64, Offset2);

    // Allocate one more space.
    UInt64 Offset3 = InvalidID;  // Start with invalid id, which is a good default since it should never happen.
    Result = FreelistAllocateBlock(&List, 64, &Offset3);
    // Verify that result is true, offset should be set to the offset+size of the previous allocation.
    ExpectToBeTrue(Result);
    ExpectShouldBe(128, Offset3);

    // Verify that the correct amount of space is free.
    UInt64 FreeSpace = FreelistFreeSpace(&List);
    ExpectShouldBe(TotalSize - 192, FreeSpace);

    // Now free the middle block.
    Result = FreelistFreeBlock(&List, 64, Offset2);
    // Verify that result is true
    ExpectToBeTrue(Result);

    // Verify the correct amount is free.
    FreeSpace = FreelistFreeSpace(&List);
    ExpectShouldBe(TotalSize - 128, FreeSpace);

    // Allocate some more space, this should fill the middle block back in.
    UInt64 Offset4 = InvalidID;  // Start with invalid id, which is a good default since it should never happen.
    Result = FreelistAllocateBlock(&List, 64, &Offset4);
    // Verify that result is true, offset should be set to the size of the previous allocation.
    ExpectToBeTrue(Result);
    ExpectShouldBe(Offset2, Offset4);  // Offset should be the same as 2 since it occupies the same space

    // Verify that the correct amount of space is free.
    FreeSpace = FreelistFreeSpace(&List);
    ExpectShouldBe(TotalSize - 192, FreeSpace);

    // Free the first block and verify space.
    Result = FreelistFreeBlock(&List, 64, Offset);
    ExpectToBeTrue(Result);
    FreeSpace = FreelistFreeSpace(&List);
    ExpectShouldBe(TotalSize - 128, FreeSpace);

    // Free the last block and verify space.
    Result = FreelistFreeBlock(&List, 64, Offset3);
    ExpectToBeTrue(Result);
    FreeSpace = FreelistFreeSpace(&List);
    ExpectShouldBe(TotalSize - 64, FreeSpace);

    // Free the middle block and verify space.
    Result = FreelistFreeBlock(&List, 64, Offset4);
    ExpectToBeTrue(Result);
    FreeSpace = FreelistFreeSpace(&List);
    ExpectShouldBe(TotalSize, FreeSpace);

    // Destroy and verify that the memory was unassigned.
    FreelistDestroy(&List);
    ExpectShouldBe(0, List.Memory);
    FMFree(Block, MemoryRequirement, MEMORY_TAG_APPLICATION);

    return true;
}

UInt8 FreelistShouldAllocateOneAndFreeMultiVaryingSizes() {
    Freelist List;

    // Get the memory requirement
    UInt64 MemoryRequirement = 0;
    UInt64 TotalSize = 512;
    FreelistCreate(TotalSize, &MemoryRequirement, 0, 0);

    // Allocate and create the freelist.
    void* Block = FMAllocate(MemoryRequirement, MEMORY_TAG_APPLICATION);
    FreelistCreate(TotalSize, &MemoryRequirement, Block, &List);

    // Allocate some space.
    UInt64 Offset = InvalidID;  // Start with invalid id, which is a good default since it should never happen.
    Bool8 Result = FreelistAllocateBlock(&List, 64, &Offset);
    // Verify that result is true, offset should be set to 0.
    ExpectToBeTrue(Result);
    ExpectShouldBe(0, Offset);

    // Allocate some more space.
    UInt64 Offset2 = InvalidID;  // Start with invalid id, which is a good default since it should never happen.
    Result = FreelistAllocateBlock(&List, 32, &Offset2);
    // Verify that result is true, offset should be set to the size of the previous allocation.
    ExpectToBeTrue(Result);
    ExpectShouldBe(64, Offset2);

    // Allocate one more space.
    UInt64 Offset3 = InvalidID;  // Start with invalid id, which is a good default since it should never happen.
    Result = FreelistAllocateBlock(&List, 64, &Offset3);
    // Verify that result is true, offset should be set to the offset+size of the previous allocation.
    ExpectToBeTrue(Result);
    ExpectShouldBe(96, Offset3);

    // Verify that the correct amount of space is free.
    UInt64 FreeSpace = FreelistFreeSpace(&List);
    ExpectShouldBe(TotalSize - 160, FreeSpace);

    // Now free the middle block.
    Result = FreelistFreeBlock(&List, 32, Offset2);
    // Verify that result is true
    ExpectToBeTrue(Result);

    // Verify the correct amount is free.
    FreeSpace = FreelistFreeSpace(&List);
    ExpectShouldBe(TotalSize - 128, FreeSpace);

    // Allocate some more space, this time larger than the old middle block. This should have a new offset
    // at the end of the list.
    UInt64 Offset4 = InvalidID;  // Start with invalid id, which is a good default since it should never happen.
    Result = FreelistAllocateBlock(&List, 64, &Offset4);
    // Verify that result is true, offset should be set to the size of the previous allocation.
    ExpectToBeTrue(Result);
    ExpectShouldBe(160, Offset4);  // Offset should be the end since it occupies more space than the old middle block.

    // Verify that the correct amount of space is free.
    FreeSpace = FreelistFreeSpace(&List);
    ExpectShouldBe(TotalSize - 192, FreeSpace);

    // Free the first block and verify space.
    Result = FreelistFreeBlock(&List, 64, Offset);
    ExpectToBeTrue(Result);
    FreeSpace = FreelistFreeSpace(&List);
    ExpectShouldBe(TotalSize - 128, FreeSpace);

    // Free the last block and verify space.
    Result = FreelistFreeBlock(&List, 64, Offset3);
    ExpectToBeTrue(Result);
    FreeSpace = FreelistFreeSpace(&List);
    ExpectShouldBe(TotalSize - 64, FreeSpace);

    // Free the middle (now end) block and verify space.
    Result = FreelistFreeBlock(&List, 64, Offset4);
    ExpectToBeTrue(Result);
    FreeSpace = FreelistFreeSpace(&List);
    ExpectShouldBe(TotalSize, FreeSpace);

    // Destroy and verify that the memory was unassigned.
    FreelistDestroy(&List);
    ExpectShouldBe(0, List.Memory);
    FMFree(Block, MemoryRequirement, MEMORY_TAG_APPLICATION);

    return true;
}

UInt8 FreelistShouldAllocateToFullAndFailToAllocateMore() {
    Freelist List;

    // Get the memory requirement
    UInt64 MemoryRequirement = 0;
    UInt64 TotalSize = 512;
    FreelistCreate(TotalSize, &MemoryRequirement, 0, 0);

    // Allocate and create the freelist.
    void* Block = FMAllocate(MemoryRequirement, MEMORY_TAG_APPLICATION);
    FreelistCreate(TotalSize, &MemoryRequirement, Block, &List);

    // Allocate all space.
    UInt64 Offset = InvalidID;  // Start with invalid id, which is a good default since it should never happen.
    Bool8 Result = FreelistAllocateBlock(&List, 512, &Offset);
    // Verify that result is true, offset should be set to 0.
    ExpectToBeTrue(Result);
    ExpectShouldBe(0, Offset);

    // Verify that the correct amount of space is free.
    UInt64 FreeSpace = FreelistFreeSpace(&List);
    ExpectShouldBe(0, FreeSpace);

    // Now try allocating some more
    UInt64 Offset2 = InvalidID;  // Start with invalid id, which is a good default since it should never happen.
    FLDEBUG("The following warning message is intentional.");
    Result = FreelistAllocateBlock(&List, 64, &Offset2);
    // Verify that result is false
    ExpectToBeFalse(Result);

    // Verify that the correct amount of space is free.
    FreeSpace = FreelistFreeSpace(&List);
    ExpectShouldBe(0, FreeSpace);

    // Destroy and verify that the memory was unassigned.
    FreelistDestroy(&List);
    ExpectShouldBe(0, List.Memory);
    FMFree(Block, MemoryRequirement, MEMORY_TAG_APPLICATION);

    return true;
}

void FreelistRegisterTests() {
    TestManagerRegisterTest(FreelistShouldCreateAndDestroy, "Freelist should create and destroy");
    TestManagerRegisterTest(FreelistShouldAllocateOneAndFreeOne, "Freelist allocate and free one entry.");
    TestManagerRegisterTest(FreelistShouldAllocateOneAndFreeMulti, "Freelist allocate and free multiple entries.");
    TestManagerRegisterTest(FreelistShouldAllocateOneAndFreeMultiVaryingSizes, "Freelist allocate and free multiple entries of varying sizes.");
    TestManagerRegisterTest(FreelistShouldAllocateToFullAndFailToAllocateMore, "Freelist allocate to full and fail when trying to allocate more.");
}