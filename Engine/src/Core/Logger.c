#include "Logger.h"
#include "Asserts.h"
#include "FString.h"
#include "Platform/Platform.h"
#include "Platform/Filesystem.h"

//NOTE: Temporary
#include <stdarg.h>

typedef struct LogState {
    FileHandle LogFileHandle;
} LogState;

static LogState* StatePtr;

void AppendToLogFile(const char* Message)
{
    UInt64 Length = StringLength(Message);
    UInt64 Written = 0;
    if (!FilesystemWrite(&StatePtr->LogFileHandle, Length, Message, &Written))
    {
        PlatformConsoleWriteError("[ERROR]: Failed writing to Console.log", LOG_ERROR);
    }
}

Bool8 LogCreate(UInt64* MemoryRequirement, void* State)
{
    *MemoryRequirement = (UInt64)sizeof(LogState);
    if (State == 0)
    {
        return true;
    }

    StatePtr = State;

    if (!FilesystemOpen("Console.log", FILE_MODE_WRITE, false, &StatePtr->LogFileHandle))
    {
        PlatformConsoleWriteError("[ERROR]: Unable to open Console.log for writing.", LOG_ERROR);
        return false;
    }

    (void)MemoryRequirement;

    return true;
}
void LogDestroy(void* State)
{
    (void)State;
    StatePtr = 0;
    //TODO: Clean Up Logging/Write queued entries.
}

void LogOutput(LogLevel Level, const char* Message, ...)
{
    static const char* LevelStrings[6] = {"[FATAL]: ", "[ERROR]: ", "[WARN]:  ", "[INFO]:  ", "[DEBUG]: ", "[TRACE]: "};
    Bool8 IsError = Level < LOG_WARN;

    constexpr UInt32 MessageLength = 32768;
    char MessageBuffer[MessageLength];
    // temp_buffer[0] = '\0';

    va_list arg_ptr;
    va_start(arg_ptr, Message);
    StringFormatV(MessageBuffer, Message, arg_ptr);
    va_end(arg_ptr);

    StringFormat(MessageBuffer, "%s%s\n", LevelStrings[Level], MessageBuffer);

    //Platform Specific Output
    if (IsError) {
        PlatformConsoleWriteError(MessageBuffer, Level);
    } else {
        PlatformConsoleWrite(MessageBuffer, Level);
    }

    // Queue a Copy to be Written to the log file
    AppendToLogFile(MessageBuffer);
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