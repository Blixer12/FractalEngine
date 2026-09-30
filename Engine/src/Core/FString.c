#include "FString.h"
#include "Core/Memory.h"

#include <string.h>

UInt64 StringLength(const char* String)
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

Bool8 StringCompare(const char* String0, const char* String1)
{
    if (!String0 || !String1) {
        return (Bool8)(String0 == String1);
    }
    return (Bool8)(strcmp(String0, String1) == 0);
}