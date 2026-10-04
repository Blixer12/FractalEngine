#pragma once

#include "Defines.h"

Bool8 PlatformSystemStartup(
    UInt64* MemoryRequirement,
    void* State,
    const char* WindowName,
    Int32 X, Int32 Y,
    Int32 Width, Int32 Height
);

void PlatformSystemShutdown(void* State);

Bool8 PlatformPollEvents();

void* PlatformAllocate(UInt64 Size, Bool8 Aligned);
void PlatformFree(void* Block, Bool8 Aligned);
void* PlatformZeroMemory(void* Block, UInt64 Size);
void* PlatformCopyMemory(void* Dest, const void* Source, UInt64 Size);
void* PlatformSetMemory(void* Dest, Int32 Value, UInt64 Size);

void PlatformConsoleWrite(const char* Message, UInt8 Color);
void PlatformConsoleWriteError(const char* Message, UInt8 Color);

Float64 PlatformGetAbsoluteTime();

void PlatformSleep(UInt64 Miliseconds);