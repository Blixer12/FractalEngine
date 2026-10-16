/**
 * @file FString.h
 * @author Blixer12
 * @brief This file contains a basic C string handling library.
 * 
 * @version 1.0
 * @date 2022-01-10
 * 
 * @copyright Fractal Engine is Copyright (c) Blixer12 2026
 * 
 */

#pragma once

#include "Defines.h"
#include "Math/MathDef.h"


/**
 * @brief Duplicates the provided string. Note that this allocates new memory,
 * which should be freed by the caller.
 * @param String The string to be duplicated.
 * @returns A pointer to a newly-created character array (string).
 */
FAPI char* StringDuplicate(const char* String);

/** 
 * @brief Gets the length of the given string.
 * @param String The string whose length to obtain.
 * @returns The length of the string.
 */
FAPI UInt64 StringLength(const char* String);

/** 
 * @brief Case-sensitive string comparison. 
 * @param String0 The first string to be compared.
 * @param String1 The second string to be compared.
 * @returns True if the same, otherwise false.
 */
FAPI Bool8 StringsEqual(const char* String0, const char* String1);

/** 
 * @brief Case-insensitive string comparison. 
 * @param String0 The first string to be compared.
 * @param String1 The second string to be compared.
 * @returns True if the same, otherwise false.
 */
FAPI Bool8 StringsEqualI(const char* String0, const char* String1);

/** 
 * @brief Performs string formatting to dest given format string and parameters.
 * @param Dest The destination for the formatted string.
 * @param Format The format string to use for the operation
 * @param ... The format arguments.
 * @returns The length of the newly-formatted string.
 */
FAPI Int32 StringFormat(char* Dest, const char* Format, ...);

/**
 * @brief Performs variadic string formatting to dest given format string and va_list.
 * @param Dest The destination for the formatted string.
 * @param Format The string to be formatted.
 * @param VaListp The variadic argument list.
 * @returns The size of the data written.
 */
FAPI Int32 StringFormatV(char* Dest, const char* Format, void* VaListp);

/**
 * @brief Empties provided string
 *
 * @param String the String to be emptied
 * @return A pointer to String
 */
FAPI char* StringEmpty(char* String);

/**
 * @brief Copies the string in source to dest. Does not perform any allocations.
 * @param Dest The destination string.
 * @param Source The source string.
 * @returns A pointer to the destination string.
 */
FAPI char* StringCopy(char* Dest, const char* Source);

/**
 * @brief Copies the string in source to dest up to the given length. Does not perform any allocations.
 * @param Dest The destination string.
 * @param Source The source string.
 * @param Length The maximum length to be copied.
 * @returns A pointer to the destination string.
 */
FAPI char* StringNcopy(char* Dest, const char* Source, Int64 Length);

/**
 * @brief Performs an in-place trim of the provided string.
 * This removes all whitespace from both ends of the string.
 * 
 * Done by placing zeroes in the string at relevant points.
 * @param String The string to be trimmed.
 * @returns A pointer to the trimmed string.
 */
FAPI char* StringTrim(char* String);

/**
 * @brief Gets a substring of the source string between start and length or to the end of the string.
 * If length is negative or 0, goes to the end of the string.
 * 
 * Done by placing zeroes in the string at relevant points.
 * @param Dest The string that gets outputted.
 * @param Source The string that gets cut
 * @param Start the placve to start copying
 * @param Length the amount that is to be copied
 */
FAPI void StringMid(char* Dest, const char* Source, Int32 Start, Int32 Length);

/**
 * @brief Returns the Index of the first ouccuranc of C in String, otherwise -1
 *
 * @param String the string to be scanned
 * @param C The character to search for
 * @return The index of the first occurance of C, otherwise -1 if not dount
 */
FAPI Int32 StringIndexOf(char* String, char C);

/**
 * @brief Attempts to parse a vector from the provided string
 *
 * @param String the string to parse from, should be space-delimited (i.e. "1.0, 2.0, 3.0, 4.0")
 * @param Vector A pointer to the vector to write to
 * @return True if parsed successfully; otherwise false
 */
FAPI Bool8 StringToVec4(char* String, Vec4* Vector);

/**
 * @brief Attempts to parse a vector from the provided string
 *
 * @param String the string to parse from, should be space-delimited (i.e. "1.0, 2.0, 3.0")
 * @param Vector A pointer to the vector to write to
 * @return True if parsed successfully; otherwise false
 */
FAPI Bool8 StringToVec3(char* String, Vec3* Vector);

/**
 * @brief Attempts to parse a vector from the provided string
 *
 * @param String the string to parse from, should be space-delimited (i.e. "1.0, 2.0")
 * @param Vector A pointer to the vector to write to
 * @return True if parsed successfully; otherwise false
 */
FAPI Bool8 StringToVec2(char* String, Vec2* Vector);

/**
 * @brief Attempts to parse a Float32 from the provided string
 *
 * @param String the string to parse from
 * @param Float A pointer to the Float to write to
 * @return True if parsed successfully; otherwise false
 */
FAPI Bool8 StringToFloat32(char* String, Float32* Float);

/**
 * @brief Attempts to parse a Float64 from the provided string
 *
 * @param String the string to parse from
 * @param Float A pointer to the Float to write to
 * @return True if parsed successfully; otherwise false
 */
FAPI Bool8 StringToFloat64(char* String, Float64* Float);

/**
 * @brief Attempts to parse a 8-bit Integer from the provided string
 *
 * @param String the string to parse from
 * @param Integer A pointer to the Integer to write to
 * @return True if parsed successfully; otherwise false
 */
FAPI Bool8 StringToInt8(char* String, Int8* Integer);

/**
 * @brief Attempts to parse a 16-bit Integer from the provided string
 *
 * @param String the string to parse from
 * @param Integer A pointer to the Integer to write to
 * @return True if parsed successfully; otherwise false
 */
FAPI Bool8 StringToInt16(char* String, Int16* Integer);

/**
 * @brief Attempts to parse a 32-bit Integer from the provided string
 *
 * @param String the string to parse from
 * @param Integer A pointer to the Integer to write to
 * @return True if parsed successfully; otherwise false
 */
FAPI Bool8 StringToInt32(char* String, Int32* Integer);

/**
 * @brief Attempts to parse a 64-bit Integer from the provided string
 *
 * @param String the string to parse from
 * @param Integer A pointer to the Integer to write to
 * @return True if parsed successfully; otherwise false
 */
FAPI Bool8 StringToInt64(char* String, Int64* Integer);

/**
 * @brief Attempts to parse a 8-bit Unsigned Integer from the provided string
 *
 * @param String the string to parse from
 * @param UnsignedInteger A pointer to the Integer to write to
 * @return True if parsed successfully; otherwise false
 */
FAPI Bool8 StringToUInt8(char* String, UInt8* UnsignedInteger);

/**
 * @brief Attempts to parse a 16-bit Unsigned Integer from the provided string
 *
 * @param String the string to parse from
 * @param UnsignedInteger A pointer to the Integer to write to
 * @return True if parsed successfully; otherwise false
 */
FAPI Bool8 StringToUInt16(char* String, UInt16* UnsignedInteger);

/**
 * @brief Attempts to parse a 32-bit Unsigned Integer from the provided string
 *
 * @param String the string to parse from
 * @param UnsignedInteger A pointer to the Integer to write to
 * @return True if parsed successfully; otherwise false
 */
FAPI Bool8 StringToUInt32(char* String, UInt32* UnsignedInteger);

/**
 * @brief Attempts to parse a 64-bit Unsigned Integer from the provided string
 *
 * @param String the string to parse from
 * @param UnsignedInteger A pointer to the Integer to write to
 * @return True if parsed successfully; otherwise false
 */
FAPI Bool8 StringToUInt64(char* String, UInt64* UnsignedInteger);

/**
 * @brief Attempts to parse a vector from the provided string
 *
 * @param String the string to parse from. "True" or "1" are true; anything false
 * @param Boolean A pointer to the Boolean to write to
 * @return True if parsed successfully; otherwise false
 */
FAPI Bool8 StringToBool(char* String, Bool8* Boolean);