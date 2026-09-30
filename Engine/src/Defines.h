#pragma once

typedef signed char Int8;
typedef short Int16;
typedef int Int32;
typedef long long Int64;

typedef unsigned char UInt8;
typedef unsigned short UInt16;
typedef unsigned int UInt32;
typedef unsigned long long UInt64;

typedef float Float32;
typedef double Float64;

// Bool Definitions
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 202311L && (defined(__clang__) || defined(__GNUC__))
    typedef bool Bool8;
#else
    typedef _Bool Bool8;

    #define true  ((Bool8)1)
    #define false ((Bool8)0)

#endif

typedef unsigned int Bool32;

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
    #define STATIC_ASSERT _Static_assert
#else
    #define STATIC_ASSERT static_assert
#endif

// Ensure all types are of the correct size
STATIC_ASSERT(sizeof(UInt8)  == 1, "Expected UInt8 to be 1 byte.");
STATIC_ASSERT(sizeof(UInt16) == 2, "Expected UInt16 to be 2 bytes.");
STATIC_ASSERT(sizeof(UInt32) == 4, "Expected UInt32 to be 4 bytes.");
STATIC_ASSERT(sizeof(UInt64) == 8, "Expected UInt64 to be 8 bytes.");

STATIC_ASSERT(sizeof(Int8)   == 1, "Expected Int8 to be 1 byte.");
STATIC_ASSERT(sizeof(Int16)  == 2, "Expected Int16 to be 2 bytes.");
STATIC_ASSERT(sizeof(Int32)  == 4, "Expected Int32 to be 4 bytes.");
STATIC_ASSERT(sizeof(Int64)  == 8, "Expected Int64 to be 8 bytes.");

STATIC_ASSERT(sizeof(Float32) == 4, "Expected Float32 to be 4 bytes.");
STATIC_ASSERT(sizeof(Float64) == 8, "Expected Float64 to be 8 bytes.");

STATIC_ASSERT(sizeof(Bool8)  == 1, "Expected Bool8 to be 1 byte");
STATIC_ASSERT(sizeof(Bool32) == 4, "Expected Bool32 to be 4 bytes");


// Platform detection
#if defined(_WIN32)
    #define FPLATFORM_WINDOWS 1
    #ifndef _WIN64
        #error "64-bit architecture is strictly required on Windows!"
    #endif
#elif defined(__APPLE__)
    #define FPLATFORM_APPLE 1
#elif defined(__linux__)
    #define FPLATFORM_LINUX 1
    #if defined(__ANDROID__)
        #define FPLATFORM_ANDROID 1
    #endif
#else
    #error "Unsupported target platform!"
#endif

// Dll Exports
#ifdef FEXPORT
#ifdef _MSC_VER
#define FAPI __declspec(dllexport)
#else
#define FAPI __attribute__((visibility("default")))
#endif
#else
// DLL Imports
#ifdef _MSC_VER
#define FAPI __declspec(dllimport)
#else
#define FAPI
#endif
#endif

#define FCLAMP(Value, Min, Max) (((Value) <= (Min)) ? (Min) : ((Value) >= (Max)) ? (Max) : (Value))

#undef UINT8_MAX
#undef UINT16_MAX
#undef UINT32_MAX
#undef UINT64_MAX

#undef INT8_MAX
#undef INT16_MAX
#undef INT32_MAX
#undef INT64_MAX

#undef INT8_MIN
#undef INT16_MIN
#undef INT32_MIN
#undef INT64_MIN

// --- Unsigned Integers Maximums ---
#define UINT8_MAX   255U
#define UINT16_MAX  65535U
#define UINT32_MAX  4294967295U
#define UINT64_MAX  18446744073709551615ULL

// --- Signed Integers Maximums ---
#define INT8_MAX    127
#define INT16_MAX   32767
#define INT32_MAX   2147483647
#define INT64_MAX   9223372036854775807LL

// --- Signed Integers Minimums ---
#define INT8_MIN    (-127 - 1)
#define INT16_MIN   (-32767 - 1)
#define INT32_MIN   (-2147483647 - 1)
#define INT64_MIN   (-9223372036854775807LL - 1)