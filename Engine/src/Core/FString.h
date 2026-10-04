#pragma once

#include "Defines.h"

FAPI char* StringDuplicate(const char* String);

FAPI UInt64 StringLength(const char* String);

FAPI Bool8 StringCompare(const char* String0, const char* String1);

// Performs string formatting to dest given format string and parameters.
FAPI Int32 StringFormat(char* Dest, const char* Format, ...);

//
/**
 * Performs variadic string formatting to dest given format string and va_list.
 * @param dest The destination for the formatted string.
 * @param format The string to be formatted.
 * @param va_list The variadic argument list.
 * @returns The size of the data written.
 */
FAPI Int32 StringFormatV(char* Dest, const char* Format, void* VaListp);