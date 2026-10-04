#include "LinearAllocatorTests.h"
#include "../TestManager.h"
#include "../Expect.h"

#include <Defines.h>

#include <Memory/LinearAllocator.h>

UInt8 LinearAllocatorShouldCreateAndDestroy() {
    LinearAllocator Allocator;
    LinearAllocatorCreate(sizeof(UInt64), 0, &Allocator);

    ExpectShouldNotBe(0, Allocator.Memory);
    ExpectShouldBe(sizeof(UInt64), Allocator.TotalSize);
    ExpectShouldBe(0, Allocator.Allocated);

    LinearAllocatorDestroy(&Allocator);

    ExpectShouldBe(0, Allocator.Memory);
    ExpectShouldBe(0, Allocator.TotalSize);
    ExpectShouldBe(0, Allocator.Allocated);

    return true;
}

UInt8 LinearAllocatorSingleAllocationAllSpace() {
    LinearAllocator Allocator;
    LinearAllocatorCreate(sizeof(UInt64), 0, &Allocator);

    // Single allocation.
    void* Block = LinearAllocatorAllocate(&Allocator, sizeof(UInt64));

    // Validate it
    ExpectShouldNotBe(0, Block);
    ExpectShouldBe(sizeof(UInt64), Allocator.Allocated);

    LinearAllocatorDestroy(&Allocator);

    return true;
}

UInt8 LinearAllocatorMultiAllocationAllSpace() {
    UInt64 MaxAllocations = 1024;
    LinearAllocator Allocator;
    LinearAllocatorCreate(sizeof(UInt64) * MaxAllocations, 0, &Allocator);

    // Multiple allocations - full.
    void* Block;
    for (UInt64 i = 0; i < MaxAllocations; ++i) {
        Block = LinearAllocatorAllocate(&Allocator, sizeof(UInt64));
        // Validate it
        ExpectShouldNotBe(0, Block);
        ExpectShouldBe(sizeof(UInt64) * (i + 1), Allocator.Allocated);
    }

    LinearAllocatorDestroy(&Allocator);

    return true;
}

UInt8 LinearAllocatorMultiAllocationOverAllocate() {
    UInt64 MaxAllocations = 3;
    LinearAllocator Allocator;
    LinearAllocatorCreate(sizeof(UInt64) * MaxAllocations, 0, &Allocator);

    // Multiple allocations - full.
    void* Block;
    for (UInt64 i = 0; i < MaxAllocations; ++i) {
        Block = LinearAllocatorAllocate(&Allocator, sizeof(UInt64));
        // Validate it
        ExpectShouldNotBe(0, Block);
        ExpectShouldBe(sizeof(UInt64) * (i + 1), Allocator.Allocated);
    }

    FLINFO("Note: The following error is intentionally caused by this test.");

    // Ask for one more allocation. Should error and return 0.
    Block = LinearAllocatorAllocate(&Allocator, sizeof(UInt64));
    // Validate it - allocated should be unchanged.
    ExpectShouldBe(0, Block);
    ExpectShouldBe(sizeof(UInt64) * (MaxAllocations), Allocator.Allocated);

    LinearAllocatorDestroy(&Allocator);

    return true;
}

UInt8 LinearAllocatorMultiAllocationAllSpaceThenFree() {
    UInt64 MaxAllocations = 1024;
    LinearAllocator Allocator;
    LinearAllocatorCreate(sizeof(UInt64) * MaxAllocations, 0, &Allocator);

    // Multiple allocations - full.
    void* Block;
    for (UInt64 i = 0; i < MaxAllocations; ++i) {
        Block = LinearAllocatorAllocate(&Allocator, sizeof(UInt64));
        // Validate it
        ExpectShouldNotBe(0, Block);
        ExpectShouldBe(sizeof(UInt64) * (i + 1), Allocator.Allocated);
    }

    // Validate that pointer is reset.
    LinearAllocatorFreeAll(&Allocator);
    ExpectShouldBe(0, Allocator.Allocated);

    LinearAllocatorDestroy(&Allocator);

    return true;
}

void LinearAllocatorRegisterTests() {
    TestManagerRegisterTest(LinearAllocatorShouldCreateAndDestroy, "Linear allocator should create and destroy");
    TestManagerRegisterTest(LinearAllocatorSingleAllocationAllSpace, "Linear allocator single Allocator for all space");
    TestManagerRegisterTest(LinearAllocatorMultiAllocationAllSpace, "Linear allocator multi Allocator for all space");
    TestManagerRegisterTest(LinearAllocatorMultiAllocationOverAllocate, "Linear allocator try over allocate");
    TestManagerRegisterTest(LinearAllocatorMultiAllocationAllSpaceThenFree, "Linear allocator allocated should be 0 after free_all");
}