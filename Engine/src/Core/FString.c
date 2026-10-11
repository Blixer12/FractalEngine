#include "FString.h"
#include "Core/Memory.h"

#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>

#ifndef _MSC_VER
#include <strings.h>
#endif

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

Bool8 StringsEqual(const char* String0, const char* String1)
{
    if (!String0 || !String1) {
        return (Bool8)(String0 == String1);
    }
    return (Bool8)(strcmp(String0, String1) == 0);
}

Bool8 StringsEqualI(const char* String0, const char* String1)
{
    #if defined(__GNUC__)
        return strcasecmp(String0, String1) == 0;
    #elif (defined _MSC_VER)
        return _strcmpi(String0, String1) == 0;
    #endif
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

char* StringEmpty(char* String)
{
    if (String)
    {
        String[0] = '\0';
    }

    return String;
}

char* StringCopy(char* Dest, const char* Source)
{
    return strcpy(Dest, Source);
}

char* StringNcopy(char* Dest, const char* Source, Int64 Length)
{
    return strncpy(Dest, Source, Length);
}

char* StringTrim(char* String)
{
    while (isspace((unsigned char)*String))
    {
        String++;
    }
    if (*String)
    {
        char* P = String;
        while (*P)
        {
            P++;
        }
        while(isspace((unsigned char)*(--P)))
        
        ;

        P[1] = '\0';
    }

    return String;
}

void StringMid(char* Dest, const char* Source, Int32 Start, Int32 Length)
{
    if (Length == 0)
    {
        return;
    }
    UInt64 SourceLength = StringLength(Source);
    if ((UInt64)Start >= SourceLength)
    {
        Dest[0] = 0;
        return;
    }
    if (Length > 0)
    {
        for (UInt64 i = Start, j = 0; j < (UInt64)Length && Source[i]; i++, j++)
        {
            Dest[j] = Source[i];
        }
        Dest[Start + Length] = 0;
    } else {
        UInt64 j = 0;
        for (UInt64 i = Start; Source[i]; i++, j++)
        {
            Dest[j] = Source[i];
        }
        Dest[Start + j] = 0;
    }
}

Int32 StringIndexOf(char* String, char C)
{
    if (!String)
    {
        return -1;
    }
    UInt32 Length = StringLength(String);
    if (Length > 0)
    {
        for (UInt32 i = 0; i < Length; i++)
        {
            if (String[i] == C)
            {
                return i;
            }
        }
    }

    return -1;
}

Bool8 StringToVec4(char* String, Vec4* Vector)
{
    if (!String)
    {
        return false;
    }

    FMZeroMemory(Vector, sizeof(Vec4));
    Int32 Result = sscanf(String, "%f %f %f %f", &Vector->x, &Vector->y, &Vector->z, &Vector->w);
    return Result != -1;
}

Bool8 StringToVec3(char* String, Vec3* Vector)
{
    if (!String)
    {
        return false;
    }

    FMZeroMemory(Vector, sizeof(Vec3));
    Int32 Result = sscanf(String, "%f %f %f", &Vector->x, &Vector->y, &Vector->z);
    return Result != -1;
}

Bool8 StringToVec2(char* String, Vec2* Vector)
{
    if (!String)
    {
        return false;
    }

    FMZeroMemory(Vector, sizeof(Vec2));
    Int32 Result = sscanf(String, "%f %f", &Vector->x, &Vector->y);
    return Result != -1;
}

Bool8 StringToFloat32(char* String, Float32* Float)
{
    if (!String)
    {
        return false;
    }

    *Float = 0;
    Int32 Result = sscanf(String, "%f", Float);
    return Result != -1;
}

Bool8 StringToFloat64(char* String, Float64* Float)
{
    if (!String)
    {
        return false;
    }

    *Float = 0;
    Int32 Result = sscanf(String, "%lf", Float);
    return Result != -1;
}

Bool8 StringToInt8(char* String, Int8* Integer)
{
    if (!String)
    {
        return false;
    }

    *Integer = 0;
    Int32 Result = sscanf(String, "%hhi", Integer);
    return Result != -1;
}

Bool8 StringToInt16(char* String, Int16* Integer)
{
    if (!String)
    {
        return false;
    }

    *Integer = 0;
    Int32 Result = sscanf(String, "%hi", Integer);
    return Result != -1;
}

Bool8 StringToInt32(char* String, Int32* Integer)
{
    if (!String)
    {
        return false;
    }

    *Integer = 0;
    Int32 Result = sscanf(String, "%i", Integer);
    return Result != -1;
}

Bool8 StringToInt64(char* String, Int64* Integer)
{
    if (!String)
    {
        return false;
    }

    *Integer = 0;
    Int32 Result = sscanf(String, "%lli", Integer);
    return Result != -1;
}

Bool8 StringToUInt8(char* String, UInt8* UnsignedInteger)
{
    if (!String)
    {
        return false;
    }

    *UnsignedInteger = 0;
    Int32 Result = sscanf(String, "%hhu", UnsignedInteger);
    return Result != -1;
}

Bool8 StringToUInt16(char* String, UInt16* UnsignedInteger)
{
    if (!String)
    {
        return false;
    }

    *UnsignedInteger = 0;
    Int32 Result = sscanf(String, "%hu", UnsignedInteger);
    return Result != -1;
}

Bool8 StringToUInt32(char* String, UInt32* UnsignedInteger)
{
    if (!String)
    {
        return false;
    }

    *UnsignedInteger = 0;
    Int32 Result = sscanf(String, "%u", UnsignedInteger);
    return Result != -1;
}

Bool8 StringToUInt64(char* String, UInt64* UnsignedInteger)
{
    if (!String)
    {
        return false;
    }

    *UnsignedInteger = 0;
    Int32 Result = sscanf(String, "%llu", UnsignedInteger);
    return Result != -1;
}

Bool8 StringToBool(char* String, Bool8* Boolean)
{
    if (!String)
    {
        return false;
    }

    *Boolean = 0;
    return StringsEqual(String, "1") || StringsEqualI(String, "true");
}