#pragma once

#include "Defines.h"

// Do disable, Comment it out
#define FASSERTIONS_ENABLED

#ifdef FASSERTIONS_ENABLED
#if _MSC_VER
#include <intrin.h>
#define DebugBreak() __debugbreak()
#else
#define debugBreak() __builtin_trap()
#endif

FAPI void AssertFail(const char* Expression, const char* Message, const char* File, Int32 Line);

#define FASSERT(Expr)                                                \
    {                                                                \
        if (Expr) {                                                  \
        } else {                                                     \
            AssertFail(#Expr, "", __FILE__, __LINE__); \
            DebugBreak();                                            \
        }                                                            \
    }

#define FASSERT_MSG(Expr, Message)                                        \
    {                                                                     \
        if (Expr) {                                                       \
        } else {                                                          \
            AssertFail(#Expr, Message, __FILE__, __LINE__); \
            DebugBreak();                                                 \
        }                                                                 \
    }

#ifdef _DEBUG
#define FASSERT_DEBUG(Expr)                                          \
    {                                                                \
        if (Expr) {                                                  \
        } else {                                                     \
            AssertFail(#Expr, "", __FILE__, __LINE__); \
            DebugBreak();                                            \
        }                                                            \
    }

#define FASSERT_DEBUG_MSG(Expr, Message)                         \
    {                                                            \
        if (Expr) {                                              \
        } else {                                                 \
            AssertFail(#Expr, Message, __FILE__, __LINE__);     \
            DebugBreak();                                        \
        }                                                        \
    }

#else
#define FASSERT_DEBUG(expr)  // Does nothing at all
#define FASSERT_DEBUG_MSG(Expr, Message)
#endif

#else
#define FASSERT(Expr)               // Does nothing at all
#define FASSERT_MSG(Expr, Message)  // Does nothing at all
#define FASSERT_DEBUG(Expr)         // Does nothing at all
#define FASSERT_DEBUG_MSG(Expr, Message)
#endif