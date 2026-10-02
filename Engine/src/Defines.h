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

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 202311L
    // C23 standard attribute
    #define MaybeUnused [[maybe_unused]]
#elif defined(__GNUC__) || defined(__clang__)
    // Fallback for older GCC/Clang compilers
    #define MaybeUnused __attribute__((unused))
#elif defined(_MSC_VER)
    // Fallback for MSVC compiler
    #define MaybeUnused __pragma(warning(suppress: 4100 4189))
#else
    #define MaybeUnused
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

#ifdef _MSC_VER
#define FINLINE __forceinline
#define FNOINLINE __declspec(noinline)
#else
#define FINLINE static inline
#define FNOINLINE
#endif