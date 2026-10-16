/**
 * @file Asserts.h
 * @author Blixer12
 * @brief This file contains assertion functions to be used throughout
 * the codebase.
 * 
 * @version 1.0
 * @date 2026-10-11
 * 
 * @copyright Fractal Engine is Copyright (c) Blixer12 2026
 * 
 */

#pragma once

#include "Defines.h"

// To disable assertions, Comment the line below out
#define FASSERTIONS_ENABLED

#ifdef FASSERTIONS_ENABLED
#if _MSC_VER
#include <intrin.h>
/** @brief Causes a debug breakpoint to be hit. */
#define DebugBreak() __debugbreak()
#else
/** @brief Causes a debug breakpoint to be hit. */
#define debugBreak() __builtin_trap()
#endif

/**
 * @brief Reports an assertion failure. Note that this is not the assertion itself,
 * just a reporting of an assertion failure that has already occurred.
 * @param Expression The expression to be reported.
 * @param Message A custom message to be reported, if provided.
 * @param File The path and name of the file containing the expression.
 * @param Line The line number in the file where the assertion failure occurred.
 */
FAPI void ReportAssertFail(const char* Expression, const char* Message, const char* File, Int32 Line);

/**
 * @brief Asserts the provided expression to be true, and logs a failure if not.
 * Also triggers a breakpoint if debugging.
 * @param Expr The expression to be evaluated.
 */
#define FASSERT(Expr)                                                \
    {                                                                \
        if (Expr) {                                                  \
        } else {                                                     \
            ReportAssertFail(#Expr, "", __FILE__, __LINE__);               \
            DebugBreak();                                            \
        }                                                            \
    }

    /**
 * @brief Asserts the provided expression to be true, and logs a failure if not.
 * Allows the user to specify a message to accompany the failure. 
 * Also triggers a breakpoint if debugging.
 * @param Expr The expression to be evaluated.
 * @param Message The message to be reported along with the assertion failure.
 */
#define FASSERT_MSG(Expr, Message)                                        \
    {                                                                     \
        if (Expr) {                                                       \
        } else {                                                          \
            ReportAssertFail(#Expr, Message, __FILE__, __LINE__);               \
            DebugBreak();                                                 \
        }                                                                 \
    }

#ifdef _DEBUG

/**
 * @brief Asserts the provided expression to be true, and logs a failure if not.
 * Also triggers a breakpoint if debugging. 
 * NOTE: Only included in debug builds; otherwise this is preprocessed out.
 * @param Expr The expression to be evaluated.
 */
#define FASSERT_DEBUG(Expr)                                          \
    {                                                                \
        if (Expr) {                                                  \
        } else {                                                     \
            ReportAssertFail(#Expr, "", __FILE__, __LINE__);               \
            DebugBreak();                                            \
        }                                                            \
    }

#define FASSERT_DEBUG_MSG(Expr, Message)                         \
    {                                                            \
        if (Expr) {                                              \
        } else {                                                 \
            ReportAssertFail(#Expr, Message, __FILE__, __LINE__);      \
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