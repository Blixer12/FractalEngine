#pragma once

#include "Defines.h"

typedef struct EventContext {
    // 256 bits
    union {
        Int64  Int64[4];
        UInt64 UInt64[4];
        Float64 Float64[4];

        Int32  Int32[8];
        UInt32 UInt32[8];
        Float32 Float32[8];

        Int16  Int16[16];
        UInt16 UInt16[16];

        Int8   Int8[32];
        UInt8  UInt8[32];

        char     Char[32];
    } Data;
} EventContext;

Bool8 EventSystemInitialize();
void EventSystemShutdown();

typedef Bool8 (*PFN_OnEvent)(UInt16 Code, void* Sender, void* ReceiverInst, EventContext Data);

FAPI Bool8 EventRegister(UInt16 Code, void* Receiver, PFN_OnEvent Callback);
FAPI Bool8 EventUnregister(UInt16 Code, void* Receiver, PFN_OnEvent Callback);
FAPI Bool8 EventFire(UInt16 Code, void* Sender, EventContext Data);

// System internal event codes. Application should use codes beyond 255.
typedef enum SystemEventCode {
    EVENT_APP_QUIT     = 0x01,
    EVENT_KEY_DOWN     = 0x02,
    EVENT_KEY_UP       = 0x03,
    EVENT_MOUSE_DOWN   = 0x04,
    EVENT_MOUSE_UP     = 0x05,
    EVENT_MOUSE_MOVE   = 0x06,
    EVENT_MOUSE_SCROLL = 0x07,
    EVENT_RESIZED      = 0x08,

    MAX_EVENT_CODE     = 0xFF
} SystemEventCode;