/**
 * @file Defines.h
 * @author Blixer12
 * @brief This file contains global type definitions which are used 
 * throughout the entire engine and applications referencing it. 
 * Numeric types are asserted statically to gurantee expected size.
 * 
 * @version 1.0
 * @date 2026-10-11
 * 
 * @copyright Fractal Engine is Copyright (c) Blixer12 2026
 * 
 */

#pragma once

/** @brief Signed 8-bit integer */
typedef signed char Int8;
/** @brief Signed 16-bit integer */
typedef short Int16;
/** @brief Signed 32-bit integer */
typedef int Int32;
/** @brief Signed 64-bit integer */
typedef long long Int64;

/** @brief Unsigned 8-bit integer */
typedef unsigned char UInt8;
/** @brief Unsigned 16-bit integer */
typedef unsigned short UInt16;
/** @brief Unsigned 32-bit integer */
typedef unsigned int UInt32;
/** @brief Unsigned 64-bit integer */
typedef unsigned long long UInt64;

/** @brief 32-bit floating point number */
typedef float Float32;
/** @brief 64-bit floating point number */
typedef double Float64;

// Bool Definitions
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 202311L && (defined(__clang__) || defined(__GNUC__))
    /** @brief 8-bit boolean type */
    typedef bool Bool8;
#else
    /** @brief 8-bit boolean type */
    typedef _Bool Bool8;

    /** @brief True... what else?*/
    #define true  ((Bool8)1)
    /** @brief False... what else?*/
    #define false ((Bool8)0)

#endif

/** @brief 32-bit boolean type, used for APIs which require it */
typedef unsigned int Bool32;

/** @brief A range, typically of memory */
typedef struct Range {
    /** @brief The offset in bytes. */
    UInt64 Offset;
    /** @brief The size in bytes. */
    UInt64 Size;
} Range;

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
    /** @brief Static assertion */
    #define STATIC_ASSERT _Static_assert
#else
    /** @brief Static assertion */
    #define STATIC_ASSERT static_assert
#endif

// Ensure all types are of the correct size

/** @brief Assert UInt8 to be 1 byte.*/
STATIC_ASSERT(sizeof(UInt8)  == 1, "Expected UInt8 to be 1 byte.");
/** @brief Assert UInt16 to be 2 byte.*/
STATIC_ASSERT(sizeof(UInt16) == 2, "Expected UInt16 to be 2 bytes.");
/** @brief Assert UInt32 to be 4 byte.*/
STATIC_ASSERT(sizeof(UInt32) == 4, "Expected UInt32 to be 4 bytes.");
/** @brief Assert UInt64 to be 8 byte.*/
STATIC_ASSERT(sizeof(UInt64) == 8, "Expected UInt64 to be 8 bytes.");

/** @brief Assert Int8 to be 1 byte.*/
STATIC_ASSERT(sizeof(Int8)   == 1, "Expected Int8 to be 1 byte.");
/** @brief Assert Int16 to be 2 byte.*/
STATIC_ASSERT(sizeof(Int16)  == 2, "Expected Int16 to be 2 bytes.");
/** @brief Assert Int32 to be 4 byte.*/
STATIC_ASSERT(sizeof(Int32)  == 4, "Expected Int32 to be 4 bytes.");
/** @brief Assert Int64 to be 8 byte.*/
STATIC_ASSERT(sizeof(Int64)  == 8, "Expected Int64 to be 8 bytes.");

/** @brief Assert Float32 to be 4 bytes.*/
STATIC_ASSERT(sizeof(Float32) == 4, "Expected Float32 to be 4 bytes.");
/** @brief Assert Float64 to be 8 bytes.*/
STATIC_ASSERT(sizeof(Float64) == 8, "Expected Float64 to be 8 bytes.");

// C23 standard version macro is 202311L (or > 201710L during draft phases)
#if !defined(__STDC_VERSION__) || __STDC_VERSION__ <= 201710L
    #if defined(_MSC_VER) && !defined(__clang__)
        // For MSVC in older C modes, static const works best for internal linkage constants
        #define constexpr static const
    #else
        // For Clang / GCC in standard C pre-C23
        #define constexpr static const
    #endif
#endif

/**
 * @brief Any ID set to this should be considered Invalid
 * and not actually pointing to a real Object (UInt32 limit, (UInt32)-1)
 */
constexpr UInt32 InvalidID = 0xFFFFFFFF;
constexpr UInt16 InvalidIDUInt16 = 0xFFFF;
constexpr UInt8 InvalidIDUInt8 = 0xFF;

// Platform detection
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
    #define FPLATFORM_WINDOWS 1
    #ifndef _WIN64
        #error "64-bit architecture is strictly required on Windows!"
    #endif
#elif defined(__APPLE__)
    #define FPLATFORM_APPLE 1
    #include <TargetConditionals.h>
    #if TARGET_IPHONE_SIMULATOR
        // iOS Simulator
        #define FPLATFORM_IOS 1
        #define FPLATFORM_IOS_SIMULATOR 1
    #elif TARGET_OS_IPHONE
        #define FPLATFORM_IOS 1
        // iOS device
    #elif TARGET_OS_MAC
        // Other kinds of Mac OS
    #else
        #error "Unknown Apple platform"
    #endif
#elif defined(__linux__) || defined(__gnu_linux__)
    #define FPLATFORM_LINUX 1
    #if defined(__ANDROID__)
        #define FPLATFORM_ANDROID 1
    #endif
#elif defined(__unix__)
    // Catch anything not caught by the above.
    #define FPLATFORM_UNIX 1
#elif defined(_POSIX_VERSION)
    // Posix
    #define FPLATFORM_POSIX 1
#else
    #error "Unsupported target platform!"
#endif

// Exports
#ifdef FEXPORT

    #ifdef _MSC_VER
        /** @brief Import/export qualifier */
        #define FAPI __declspec(dllexport)
    #else
        /** @brief Import/export qualifier */
        #define FAPI __attribute__((visibility("default")))
    #endif

#else
    // DLL Imports
    #ifdef _MSC_VER
        /** @brief Import/export qualifier */
        #define FAPI __declspec(dllimport)
    #else
        /** @brief Import/export qualifier */
        #define FAPI
    #endif
#endif

/**
 * @brief Clamps value to a range of min and max (inclusive).
 * @param Value The value to be clamped.
 * @param Min The minimum value of the range.
 * @param Max The maximum value of the range.
 * @returns The clamped value.
 */
#define FCLAMP(Value, Min, Max) (((Value) <= (Min)) ? (Min) : ((Value) >= (Max)) ? (Max) : (Value))

#ifdef _MSC_VER
/** @brief Inline qualifier */
#define FINLINE static __forceinline
/** @brief No-inline qualifier */
#define FNOINLINE __declspec(noinline)
#else
/** @brief Inline qualifier */
#define FINLINE static inline
/** @brief No-inline qualifier */
#define FNOINLINE
#endif

/** @brief Gets the number of bytes from amount of gibibytes (GiB) (1024*1024*1024) */
#define GIBIBYTES(amount) amount * 1024 * 1024 * 1024
/** @brief Gets the number of bytes from amount of mebibytes (MiB) (1024*1024) */
#define MEBIBYTES(amount) amount * 1024 * 1024
/** @brief Gets the number of bytes from amount of kibibytes (KiB) (1024) */
#define KIBIBYTES(amount) amount * 1024

/** @brief Gets the number of bytes from amount of gigabytes (GB) (1000*1000*1000) */
#define GIGABYTES(amount) amount * 1000 * 1000 * 1000
/** @brief Gets the number of bytes from amount of megabytes (MB) (1000*1000) */
#define MEGABYTES(amount) amount * 1000 * 1000
/** @brief Gets the number of bytes from amount of kilobytes (KB) (1000) */
#define KILOBYTES(amount) amount * 1000


FINLINE UInt64 GetAligned(UInt64 Operand, UInt64 Granularity) {
    return ((Operand + (Granularity - 1)) & ~(Granularity - 1));
}

FINLINE Range GetAlignedRange(UInt64 Offset, UInt64 Size, UInt64 Granularity) {
    return (Range){GetAligned(Offset, Granularity), GetAligned(Size, Granularity)};
}