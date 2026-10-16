/**
 * @file Logger.h
 * @author Blixer12
 * @brief This file contains structures and logic pertaining to the logging system.
 * 
 * @version 1.0
 * @date 2026-10-11
 * 
 * @copyright Fractal Engine is Copyright (c) Blixer12 2026
 * 
 */

#pragma once

#include "Defines.h"

/** @brief Indicates if warning level logging is enabled. */
#define LOG_WARN_ENABLED 1
/** @brief Indicates if info level logging is enabled. */
#define LOG_INFO_ENABLED 1
/** @brief Indicates if debug level logging is enabled. */
#define LOG_DEBUG_ENABLED 1
/** @brief Indicates if trace level logging is enabled. */
#define LOG_TRACE_ENABLED 1


#if FRELEASE == 1
#define LOG_DEBUG_ENABLED 0
#define LOG_TRACE_ENABLED 0
#endif

typedef enum LogLevel
{
    /** @brief Fatal log level, should be used to stop the application when hit. */
    LOG_FATAL = 0,
    /** @brief Error log level, should be used to indicate critical runtime problems that cause the application to run improperly or not at all. */
    LOG_ERROR = 1,
    /** @brief Warning log level, should be used to indicate non-critial problems with the application that cause it to run suboptimally. */
    LOG_WARN = 2,
    /** @brief Info log level, should be used for non-erronuous informational purposes. */
    LOG_INFO = 3,
    /** @brief Debug log level, should be used for debugging purposes. */
    LOG_DEBUG = 4,
    /** @brief Trace log level, should be used for verbose debugging purposes. */
    LOG_TRACE = 5
} LogLevel;


/**
 * @brief Creates the log system. Call twice; once to obtain memory requirement (passing
 * state = 0), then a second time passing allocated memory to state.
 * 
 * @param MemoryRequirement The required size of the state memory.
 * @param State Either 0 or the allocated block of state memory.
 * @return Bool8 True on success; otherwise false.
 */
Bool8 LogCreate(UInt64* MemoryRequirement, void* State);

/**
 * @brief Shuts down the logging system.
 * @param State A pointer to the system state.
 */
void LogDestroy(void* State);

/**
 * @brief Outputs logging at the given level.
 * @param Level The log level to use.
 * @param Message The message to be logged.
 * @param ... Any formatted data that should be included in the log entry.
 */
FAPI void LogOutput(LogLevel Level, const char* Message, ...);

#ifndef FLFATAL
/** 
 * @brief Logs a fatal-level message. Should be used to stop the application when hit.
 * @param Message The message to be logged. Can be a format string for additional parameters.
 * @param ... Additional parameters to be logged.
 */
#define FLFATAL(Message, ...) LogOutput(LOG_FATAL, Message, ##__VA_ARGS__)
#endif

#ifndef FLERROR
/** 
 * @brief Logs an error-level message. Should be used to indicate critical runtime problems 
 * that cause the application to run improperly or not at all.
 * @param Message The message to be logged.
 * @param ... Any formatted data that should be included in the log entry.
 */
#define FLERROR(Message, ...) LogOutput(LOG_ERROR, Message, ##__VA_ARGS__)
#endif
#if LOG_WARN_ENABLED == 1

/** 
 * @brief Logs a warning-level message. Should be used to indicate non-critial problems with 
 * the application that cause it to run suboptimally.
 * @param Message The message to be logged.
 * @param ... Any formatted data that should be included in the log entry.
 */
#define FLWARN(Message, ...) LogOutput(LOG_WARN, Message, ##__VA_ARGS__)

#else 
/** 
 * @brief Logs a warning-level message. Should be used to indicate non-critial problems with 
 * the application that cause it to run suboptimally. Does nothing when LOG_WARN_ENABLED != 1
 * @param Message The message to be logged.
 * @param ... Any formatted data that should be included in the log entry.
 */
#define FLWARN(Message, ...)
#endif

// Logs any Information
#if LOG_INFO_ENABLED == 1

/** 
 * @brief Logs an info-level message. Should be used for non-erronuous informational purposes.
 * @param Message The message to be logged.
 * @param ... Any formatted data that should be included in the log entry.
 */
#define FLINFO(Message, ...) LogOutput(LOG_INFO, Message, ##__VA_ARGS__)


#else 
/** 
 * @brief Logs an info-level message. Should be used for non-erronuous informational purposes.
 * @param Message The message to be logged.
 * @param ... Any formatted data that should be included in the log entry.
 */
#define FLINFO(Message, ...)
#endif

// Logs any Debug Info
#if LOG_DEBUG_ENABLED == 1

/** 
 * @brief Logs a debug-level message. Should be used for debugging purposes.
 * @param Message The message to be logged.
 * @param ... Any formatted data that should be included in the log entry.
 */
#define FLDEBUG(Message, ...) LogOutput(LOG_DEBUG, Message, ##__VA_ARGS__)


#else 
/** 
 * @brief Logs a debug-level message. Should be used for debugging purposes.
 * @param Message The message to be logged.
 * @param ... Any formatted data that should be included in the log entry.
 */
#define FLDEBUG(Message, ...)
#endif

// Logs any Debug Info
#if LOG_TRACE_ENABLED == 1

/** 
 * @brief Logs a trace-level message. Should be used for verbose debugging purposes.
 * @param Message The message to be logged.
 * @param ... Any formatted data that should be included in the log entry.
 */
#define FLTRACE(Message, ...) LogOutput(LOG_TRACE, Message, ##__VA_ARGS__)


#else 
/** 
 * @brief Logs a trace-level message. Should be used for verbose debugging purposes.
 * @param Message The message to be logged.
 * @param ... Any formatted data that should be included in the log entry.
 */
#define FLTRACE(Message, ...)
#endif