#include "Logger.h"
#include "Asserts.h"
#include "Platform/Platform.h"

//NOTE: Temporary
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

Bool8 LogCreate()
{
    //TODO: Create a Log file.
    return true;
}
void LogShutdown()
{
    //TODO: Clean Up Logging/Write queued entries.
}

void LogOutput(LogLevel Level, const char* Message, ...)
{
    static const char* level_strings[6] = {"[FATAL]: ", "[ERROR]: ", "[WARN]:  ", "[INFO]:  ", "[DEBUG]: ", "[TRACE]: "};
    Bool8 IsError = Level < LOG_WARN;

    enum { MessageLength = 32768 };
    static char TempBuffer[MessageLength];
    // temp_buffer[0] = '\0';

    va_list arg_ptr;
    va_start(arg_ptr, Message);
    vsnprintf(TempBuffer, 32768, Message, arg_ptr);
    va_end(arg_ptr);

    static char FinalBuffer[MessageLength];
    snprintf(FinalBuffer, sizeof(FinalBuffer), "%s%s\n", level_strings[Level], TempBuffer);

    //Platform Specific Output
    if (IsError) {
        PlatformConsoleWriteError(FinalBuffer, Level);
    } else {
        PlatformConsoleWrite(FinalBuffer, Level);
    }
}

void AssertFail(const char* Expression, const char* Message, const char* File, Int32 Line)
{
    LogOutput(
        LOG_FATAL, 
        "Assertion Failure: %s\n"
        "Message: '%s'\n"
        "File:    %s\n"
        "Line:    %d\n", 
        Expression, Message, File, Line
    );
}