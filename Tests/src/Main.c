#include "TestManager.h"

#include "Memory/LinearAllocatorTests.h"

#include <Core/Logger.h>

int main() {
    // Always initalize the test manager first.
    TestManagerInit();

    // TODO: add test registrations here.
    LinearAllocatorRegisterTests();


    FLDEBUG("Starting tests...");

    // Execute tests
    TestManagerRunTests();

    return 0;
}