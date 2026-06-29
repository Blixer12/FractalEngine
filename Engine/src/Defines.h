#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef int8_t   Int8;
typedef int16_t  Int16;
typedef int32_t  Int32;
typedef int64_t  Int64;

typedef uint8_t  UInt8;
typedef uint16_t UInt16;
typedef uint32_t UInt32;
typedef uint64_t UInt64;

typedef float    Float32;
typedef double   Float64;

typedef uint32_t Bool32;
typedef uint8_t Bool8;

// Checks if the size is Correct
static_assert(sizeof(Bool8)  == 1, "Expected Bool8 to be 1 byte");
static_assert(sizeof(Bool32) == 4, "Expected Bool32 to be 4 bytes");

#define TRUE  1
#define FALSE 0


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