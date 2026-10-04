#pragma once

#include <Defines.h>

#define BYPASS 2

typedef UInt8 (*PFN_Test)();

void TestManagerInit();

void TestManagerRegisterTest(PFN_Test, char* Description);

void TestManagerRunTests();