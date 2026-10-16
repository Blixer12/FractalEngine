#include "TestManager.h"

#include "Memory/LinearAllocatorTests.h"
#include "Memory/DynamicAllocatorTests.h"

#include "Containers/HashtableTests.h"
#include "Containers/FreelistTests.h"

#include <Core/Logger.h>

int main() 
{
    // Always initalize the test manager first.
    TestManagerInit();

    // TODO: add test registrations here.
    LinearAllocatorRegisterTests();

    HashtableRegisterTests();

    FreelistRegisterTests();

    DynamicAllocatorRegisterTests();

    FLDEBUG("Starting tests...");

    // Execute tests
    TestManagerRunTests();

    return 0;
}