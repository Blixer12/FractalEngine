#pragma once

#include "Defines.h"
#include "Math/MathDef.h"

FAPI char* StringDuplicate(const char* String);

FAPI UInt64 StringLength(const char* String);

FAPI Bool8 StringsEqual(const char* String0, const char* String1);

// Case-insensitive string comparison. True if the same, otherwise false.
FAPI Bool8 StringsEqualI(const char* String0, const char* String1);

// Performs string formatting to dest given format string and parameters.
FAPI Int32 StringFormat(char* Dest, const char* Format, ...);

/**
 * Performs variadic string formatting to dest given format string and va_list.
 * @param dest The destination for the formatted string.
 * @param format The string to be formatted.
 * @param va_list The variadic argument list.
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

FAPI char* StringCopy(char* Dest, const char* Source);

FAPI char* StringNcopy(char* Dest, const char* Source, Int64 Length);

FAPI char* StringTrim(char* String);

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