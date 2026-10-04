#include "TestManager.h"

#include <Containers/Vector.h>
#include <Core/Logger.h>
#include <Core/FString.h>
#include <Core/Clock.h>

typedef struct TestEntry {
    PFN_Test Function;
    char* Description;
} TestEntry;

static TestEntry* Tests;

void TestManagerInit() {
    Tests = VectorCreate(TestEntry);
}

void TestManagerRegisterTest(UInt8 (*PFN_Test)(), char* Description) {
    TestEntry Test;
    Test.Function = PFN_Test;
    Test.Description = Description;
    VectorAppend(Tests, Test);
}

void TestManagerRunTests() {
    UInt32 Passed = 0;
    UInt32 Failed = 0;
    UInt32 Skipped = 0;

    UInt32 Count = VectorSize(Tests);

    Clock TotalTime;
    ClockStart(&TotalTime);

    for (UInt32 i = 0; i < Count; ++i) {
        Clock TestTime;
        ClockStart(&TestTime);
        UInt8 Result = Tests[i].Function();
        ClockUpdate(&TestTime);

        if (Result == true) {
            ++Passed;
        } else if (Result == BYPASS) {
            FLWARN("[SKIPPED]: %s", Tests[i].Description);
            ++Skipped;
        } else {
            FLERROR("[FAILED]: %s", Tests[i].Description);
            ++Failed;
        }
        char Status[20];
        StringFormat(Status, Failed ? "*** %d FAILED ***" : "SUCCESS", Failed);
        ClockUpdate(&TotalTime);
        FLINFO("Executed %d of %d (Skipped %d) %s (%.6f sec / %.6f sec total", i + 1, Count, Skipped, Status, TestTime.Elapsed, TotalTime.Elapsed);
    }

    ClockStop(&TotalTime);

    FLINFO("Results: %d Passed, %d Failed, %d Skipped.", Passed, Failed, Skipped);
}