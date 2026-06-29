#include "String.h"
#include "Core/Memory.h"

#include <string.h>

FAPI UInt64 StringLength(const char* String)
{
    return strlen(String);
}

char* StringDuplicate(const char* String)
{
    UInt64 Length = StringLength(String);
    char* Copy = FMAllocate(Length + 1, MEMORY_TAG_STRING);
    FMCopyMemory(Copy, String, Length + 1);
    return Copy;
}