#pragma once

#include "Defines.h"

FAPI char* StringDuplicate(const char* String);

FAPI UInt64 StringLength(const char* String);

FAPI Bool8 StringCompare(const char* String0, const char* String1);