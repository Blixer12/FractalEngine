#include "DynamicAllocatorTests.h"
#include "../TestManager.h"
#include "../Expect.h"

#include <Defines.h>
#include <Core/Memory.h>
#include <Memory/DynamicAllocator.h>

UInt8 DynamicAllocatorShouldCreateAndDestroy() {
    DynamicAllocator Alloc;
    UInt64 MemoryRequirement = 0;
    // Get the memory requirement
    Bool8 Result = DynamicAllocatorCreate(1024, &MemoryRequirement, 0, 0);
    ExpectToBeTrue(Result);

    // Actually create the allocator.
    void* Memory = FMAllocate(MemoryRequirement, MEMORY_TAG_APPLICATION);
    Result = DynamicAllocatorCreate(1024, &MemoryRequirement, Memory, &Alloc);
    ExpectToBeTrue(Result);
    ExpectShouldNotBe(0, Alloc.Memory);
    UInt64 FreeSpace = DynamicAllocatorFreeSpace(&Alloc);
    ExpectShouldBe(1024, FreeSpace);

    // Destroy the allocator.
    DynamicAllocatorDestroy(&Alloc);
    ExpectShouldBe(0, Alloc.Memory);
    FMFree(Memory, MemoryRequirement, MEMORY_TAG_APPLICATION);
    return true;
}

UInt8 DynamicAllocatorSingleAllocationAllSpace() {
    DynamicAllocator Alloc;
    UInt64 MemoryRequirement = 0;
    // Get the memory requirement
    Bool8 Result = DynamicAllocatorCreate(1024, &MemoryRequirement, 0, 0);
    ExpectToBeTrue(Result);

    // Actually create the allocator.
    void* Memory = FMAllocate(MemoryRequirement, MEMORY_TAG_APPLICATION);
    Result = DynamicAllocatorCreate(1024, &MemoryRequirement, Memory, &Alloc);
    ExpectToBeTrue(Result);
    ExpectShouldNotBe(0, Alloc.Memory);
    UInt64 FreeSpace = DynamicAllocatorFreeSpace(&Alloc);
    ExpectShouldBe(1024, FreeSpace);

    // Allocate the whole thing.
    void* Block = DynamicAllocatorAllocate(&Alloc, 1024);
    ExpectShouldNotBe(0, Block);

    // Verify free space
    FreeSpace = DynamicAllocatorFreeSpace(&Alloc);
    ExpectShouldBe(0, FreeSpace);

    // Free the allocation
    DynamicAllocatorFree(&Alloc, Block, 1024);

    // Verify free space.
    FreeSpace = DynamicAllocatorFreeSpace(&Alloc);
    ExpectShouldBe(1024, FreeSpace);

    // Destroy the allocator.
    DynamicAllocatorDestroy(&Alloc);
    ExpectShouldBe(0, Alloc.Memory);
    FMFree(Memory, MemoryRequirement, MEMORY_TAG_APPLICATION);
    return true;
}

UInt8 DynamicAllocatorMultiAllocationAllSpace() {
    DynamicAllocator Alloc;
    UInt64 MemoryRequirement = 0;
    // Get the memory requirement
    Bool8 Result = DynamicAllocatorCreate(1024, &MemoryRequirement, 0, 0);
    ExpectToBeTrue(Result);

    // Actually create the allocator.
    void* Memory = FMAllocate(MemoryRequirement, MEMORY_TAG_APPLICATION);
    Result = DynamicAllocatorCreate(1024, &MemoryRequirement, Memory, &Alloc);
    ExpectToBeTrue(Result);
    ExpectShouldNotBe(0, Alloc.Memory);
    UInt64 FreeSpace = DynamicAllocatorFreeSpace(&Alloc);
    ExpectShouldBe(1024, FreeSpace);

    // Allocate part of the block.
    void* Block = DynamicAllocatorAllocate(&Alloc, 256);
    ExpectShouldNotBe(0, Block);

    // Verify free space
    FreeSpace = DynamicAllocatorFreeSpace(&Alloc);
    ExpectShouldBe(768, FreeSpace);

    // Allocate another part of the block.
    void* Block2 = DynamicAllocatorAllocate(&Alloc, 512);
    ExpectShouldNotBe(0, Block2);

    // Verify free space
    FreeSpace = DynamicAllocatorFreeSpace(&Alloc);
    ExpectShouldBe(256, FreeSpace);

    // Allocate the last part of the block.
    void* Block3 = DynamicAllocatorAllocate(&Alloc, 256);
    ExpectShouldNotBe(0, Block3);

    // Verify free space
    FreeSpace = DynamicAllocatorFreeSpace(&Alloc);
    ExpectShouldBe(0, FreeSpace);

    // Free the allocations, out of order, and verify free space
    DynamicAllocatorFree(&Alloc, Block3, 256);

    // Verify free space.
    FreeSpace = DynamicAllocatorFreeSpace(&Alloc);
    ExpectShouldBe(256, FreeSpace);

    // Free the next allocation, out of order
    DynamicAllocatorFree(&Alloc, Block, 256);

    // Verify free space.
    FreeSpace = DynamicAllocatorFreeSpace(&Alloc);
    ExpectShouldBe(512, FreeSpace);

    // Free the final allocation, out of order
    DynamicAllocatorFree(&Alloc, Block2, 512);

    // Verify free space.
    FreeSpace = DynamicAllocatorFreeSpace(&Alloc);
    ExpectShouldBe(1024, FreeSpace);

    // Destroy the allocator.
    DynamicAllocatorDestroy(&Alloc);
    ExpectShouldBe(0, Alloc.Memory);
    FMFree(Memory, MemoryRequirement, MEMORY_TAG_APPLICATION);
    return true;
}

void DynamicAllocatorRegisterTests() {
    TestManagerRegisterTest(DynamicAllocatorShouldCreateAndDestroy, "Dynamic allocator should create and destroy");
    TestManagerRegisterTest(DynamicAllocatorSingleAllocationAllSpace, "Dynamic allocator single alloc for all space");
    TestManagerRegisterTest(DynamicAllocatorMultiAllocationAllSpace, "Dynamic allocator multi alloc for all space");
}