#pragma once

#include "Defines.h"

#define LOG_WARN_ENABLED 1
#define LOG_INFO_ENABLED 1
#define LOG_DEBUG_ENABLED 1
#define LOG_TRACE_ENABLED 1

#if FRELEASE == 1
#define LOG_DEBUG_ENABLED 0
#define LOG_TRACE_ENABLED 0
#endif

typedef enum LogLevel
{
    LOG_FATAL = 0,
    LOG_ERROR = 1,
    LOG_WARN = 2,
    LOG_INFO = 3,
    LOG_DEBUG = 4,
    LOG_TRACE = 5
} LogLevel;


Bool8 LogCreate();
void LogShutdown();

FAPI void LogOutput(LogLevel Level, const char* Message, ...);

//Logs a Fatal Error
#ifndef FLFATAL
#define FLFATAL(Message, ...) LogOutput(LOG_FATAL, Message, ##__VA_ARGS__)
#endif

// Logs an Error
#ifndef FLERROR
#define FLERROR(Message, ...) LogOutput(LOG_ERROR, Message, ##__VA_ARGS__)
#endif

// Logs any Warnings
#if LOG_WARN_ENABLED == 1

#ifndef FLWARN
#define FLWARN(Message, ...) LogOutput(LOG_WARN, Message, ##__VA_ARGS__)
#endif

#else 
#define FLWARN(Message, ...)
#endif

// Logs any Information
#if LOG_INFO_ENABLED == 1

#ifndef FLINFO
#define FLINFO(Message, ...) LogOutput(LOG_INFO, Message, ##__VA_ARGS__)
#endif

#else 
#define FLINFO(Message, ...)
#endif

// Logs any Debug Info
#if LOG_DEBUG_ENABLED == 1

#ifndef FLDEBUG
#define FLDEBUG(Message, ...) LogOutput(LOG_DEBUG, Message, ##__VA_ARGS__)
#endif

#else 
#define FLDEBUG(Message, ...)
#endif

// Logs any Debug Info
#if LOG_TRACE_ENABLED == 1

#ifndef FLTRACE
#define FLTRACE(Message, ...) LogOutput(LOG_TRACE, Message, ##__VA_ARGS__)
#endif

#else 
#define FLTRACE(Message, ...)
#endif