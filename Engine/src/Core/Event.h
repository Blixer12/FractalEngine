/**
 * @file Event.h
 * @author Blixer12
 * @brief This file contains structures and functions specific to the
 * event system. 
 * Events are a mechenism that allows the developer to send and recieve
 * data at critical points in the execution of the application in a non-
 * coupled way. For now, this follows a simple pub-sub model of event
 * transmission.
 * 
 * @version 1.0
 * @date 2026-10-11
 * 
 * @copyright Fractal Engine is Copyright (c) Blixer12 2026
 * 
 */

#pragma once

#include "Defines.h"

/**
 * @brief Represents event contextual data to be sent along with an
 * event code when an event is fired.
 * It is a union that is 256 bits in size, meaning data can be mixed
 * and matched as required by the developer.
 * */
typedef struct EventContext {
    // 256 bits
    union {
        /** @brief An array of 4 64-bit signed integers. */
        Int64   Int64[4];
        /** @brief An array of 4 64-bit unsigned integers. */
        UInt64  UInt64[4];
        /** @brief An array of 4 64-bit floating-point numbers. */
        Float64 Float64[4];

        /** @brief An array of 8 32-bit signed integers. */
        Int32   Int32[8];
        /** @brief An array of 8 32-bit unsigned integers. */
        UInt32  UInt32[8];
        /** @brief An array of 8 32-bit floating-point numbers. */
        Float32 Float32[8];

        /** @brief An array of 16 16-bit signed integers. */
        Int16   Int16[16];
        /** @brief An array of 16 16-bit unsigned integers. */
        UInt16  UInt16[16];

        /** @brief An array of 32 8-bit unsigned integers. */
        Int8    Int8[32];
        /** @brief An array of 32 8-bit unsigned integers. */
        UInt8   UInt8[32];

        /** @brief An array of 32 characters. */
        char    Char[32];
    } Data;
} EventContext;

/**
 * @brief Initializes the event system. Call twice; once to obtain memory requirement (passing
 * state = 0), then a second time passing allocated memory to state.
 * 
 * @param MemoryRequirement The required size of the state memory.
 * @param State Either 0 or the allocated block of state memory.
 */
void EventSystemInitialize(UInt64* MemoryRequirement, void* State);

/**
 * @brief Shuts the event system
 * 
 * @param State what to 0 out
 */
void EventSystemShutdown(void* State);


/**
 * @brief A function pointer typedef which is used for event subscriptions by the subscriber.
 * @param Code The event code to be sent.
 * @param Sender A pointer to the sender of the event. Can be 0.
 * @param ReceiverInst A pointer to the Reciever of the event. Can be 0.
 * @param Data The event context to be passed with the fired event.
 * @returns True if the message should be considered handled, which means that it will not
 * be sent to any other consumers; otherwise false.
 */
typedef Bool8 (*PFN_OnEvent)(UInt16 Code, void* Sender, void* ReceiverInst, EventContext Data);

/**
 * @brief Register to listen for when events are sent with the provided code. Events with duplicate
 * listener/callback combos will not be registered again and will cause this to return false.
 * @param Code The event code to listen for.
 * @param Receiver A pointer to a listener instance. Can be 0/NULL.
 * @param Callback The callback function pointer to be invoked when the event code is fired.
 * @returns True if the event is successfully registered; otherwise false.
 */
FAPI Bool8 EventRegister(UInt16 Code, void* Receiver, PFN_OnEvent Callback);

/**
 * @brief Unregister from listening for when events are sent with the provided code. If no matching
 * registration is found, this function returns false.
 * @param Code The event code to stop listening for.
 * @param Receiver A pointer to a listener instance. Can be 0/NULL.
 * @param Callback The callback function pointer to be unregistered.
 * @returns True if the event is successfully deregistered; otherwise false.
 */
FAPI Bool8 EventUnregister(UInt16 Code, void* Receiver, PFN_OnEvent Callback);

/**
 * @brief Fires an event to listeners of the given code. If an event handler returns 
 * true, the event is considered handled and is not passed on to any more listeners.
 * @param Code The event code to fire.
 * @param Sender A pointer to the sender. Can be 0/NULL.
 * @param Data The event data.
 * @returns True if handled, otherwise false.
 */
FAPI Bool8 EventFire(UInt16 Code, void* Sender, EventContext Data);

/** @brief System internal event codes. Application should use codes beyond 255. */
typedef enum SystemEventCode {
     /** @brief Shuts the application down on the next frame. */
    EVENT_APP_QUIT     = 0x01,

    /** @brief Keyboard key pressed.
     * Context usage:
     * UInt16 KeyCode = Data.Data.UInt16[0];
     */
    EVENT_KEY_DOWN     = 0x02,
    
    /** @brief Keyboard key released.
     * Context usage:
     * UInt16 KeyCode = Data.Data.UInt16[0];
     */
    EVENT_KEY_UP       = 0x03,


    /** @brief Mouse button pressed.
     * Context usage:
     * UInt16 Button = Data.Data.UIn16[0];
     */
    EVENT_MOUSE_DOWN   = 0x04,

    /** @brief Mouse button released.
     * Context usage:
     * UInt16 button = Data.Data.UIn16[0];
     */
    EVENT_MOUSE_UP     = 0x05,

    /** @brief Mouse moved.
     * Context usage:
     * UInt16 x = Data.Data.UInt16[0];
     * UInt16 y = Data.Data.UInt16[1];
     */
    EVENT_MOUSE_MOVE   = 0x06,

    /** @brief Mouse moved.
     * Context usage:
     * UInt8 WheelDelta = Data.Data.UInt8[0];
     */
    EVENT_MOUSE_SCROLL = 0x07,


    /** @brief Resized/resolution changed from the OS.
     * Context usage:
     * UInt16 Width = Data.Data.UInt16[0];
     * UInt16 Height = Data.Data.UInt16[1];
     */
    EVENT_RESIZED      = 0x08,

    /** @brief Special-purpose debugging event. Context will vary over time. */
    EVENT_DEBUG0       = 0x10,

    /** @brief Special-purpose debugging event. Context will vary over time. */
    EVENT_DEBUG1       = 0x11,

    /** @brief Special-purpose debugging event. Context will vary over time. */
    EVENT_DEBUG2       = 0x12,

    /** @brief Special-purpose debugging event. Context will vary over time. */
    EVENT_DEBUG3       = 0x13,

    /** @brief Special-purpose debugging event. Context will vary over time. */
    EVENT_DEBUG4       = 0x14,

    /** @brief The maximum event code that can be used internally. */
    MAX_EVENT_CODE     = 0xFF
} SystemEventCode;