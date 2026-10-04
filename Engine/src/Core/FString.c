#include "FString.h"
#include "Core/Memory.h"

#include <string.h>
#include <stdio.h>
#include <stdarg.h>

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

Int32 StringFormat(char* Dest, const char* Format, ...) {
    if (Dest) {
        __builtin_va_list ArgPtr;
        va_start(ArgPtr, Format);
        Int32 Written = StringFormatV(Dest, Format, ArgPtr);
        va_end(ArgPtr);
        return Written;
    }
    return -1;
}

Int32 StringFormatV(char* Dest, const char* Format, void* VaListp) {
    if (Dest) {
        // Big, but can fit on the stack.
        char Buffer[32768];
        Int32 Written = vsnprintf(Buffer, 32768, Format, VaListp);
        Buffer[Written] = 0;
        FMCopyMemory(Dest, Buffer, Written + 1);

        return Written;
    }
    return -1;
}