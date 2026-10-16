/**
 * @file Platform.h
 * @author Blixer12
 * @brief This file contains the platform layer, or at least the interface to it.
 * Each platform should provide its own implementation of this in a .c file, and 
 * should be compiled exclusively of the rest.
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
 * @brief Performs startup routines within the platform layer. Should be called twice,
 * once to obtain the memory requirement (with state=0), then a second time passing 
 * an allocated block of memory to state.
 * 
 * @param MemoryRequirement A pointer to hold the memory requirement in bytes.
 * @param State A pointer to a block of memory to hold state. If obtaining memory requirement only, pass 0.
 * @param WindowName The name of the window/app.
 * @param X The initial x position of the main window.
 * @param Y The initial y position of the main window.
 * @param Width The initial width of the main window.
 * @param Height The initial height of the main window.
 * @return True on success; otherwise false.
 */
Bool8 PlatformSystemStartup(
    UInt64* MemoryRequirement,
    void* State,
    const char* WindowName,
    Int32 X, Int32 Y,
    Int32 Width, Int32 Height
);

/**
 * @brief Shuts down the platform layer.
 * 
 * @param State A pointer to the platform layer state.
 */
void PlatformSystemShutdown(void* State);

/**
 * @brief Performs any platform-specific message pumping that is required
 * for windowing, etc.
 * 
 * @return True on success; otherwise false.
 */
Bool8 PlatformPollEvents();

/**
 * @brief Performs platform-specific memory allocation of the given size.
 * 
 * @param Size The size of the allocation in bytes.
 * @param Aligned Indicates if the allocation should be aligned.
 * @return A pointer to a block of allocated memory.
 */
void* PlatformAllocate(UInt64 Size, Bool8 Aligned);

/**
 * @brief Frees the given block of memory.
 * 
 * @param Block The block to be freed.
 * @param Aligned Indicates if the block of memory is aligned.
 */
void PlatformFree(void* Block, Bool8 Aligned);

/**
 * @brief Performs platform-specific zeroing out of the given block of memory.
 * 
 * @param Block The block to be zeroed out.
 * @param Size The size of data to zero out.
 * @return A pointer to the zeroed out block of memory.
 */
void* PlatformZeroMemory(void* Block, UInt64 Size);

/**
 * @brief Copies the bytes of memory in source to dest, of the given size.
 * 
 * @param Dest The destination memory block.
 * @param Source The source memory block.
 * @param Size The size of data to be copied.
 * @return A pointer to the destination block of memory.
 */
void* PlatformCopyMemory(void* Dest, const void* Source, UInt64 Size);

/**
 * @brief Sets the bytes of memory to the given value.
 * 
 * @param Dest The destination block of memory.
 * @param Value The value to be set.
 * @param Size The size of data to set.
 * @return A pointer to the set block of memory.
 */
void* PlatformSetMemory(void* Dest, Int32 Value, UInt64 Size);

/**
 * @brief Performs platform-specific printing to the console of the given 
 * message and color code (if supported).
 * 
 * @param Message The message to be printed.
 * @param Color The color to print the text in (if supported).
 */
void PlatformConsoleWrite(const char* Message, UInt8 Color);

/**
 * @brief Performs platform-specific printing to the error console of the given 
 * message and color code (if supported).
 * 
 * @param Message The message to be printed.
 * @param Color The color to print the text in (if supported).
 */
void PlatformConsoleWriteError(const char* Message, UInt8 Color);

/**
 * @brief Gets the absolute time since the application started.
 * 
 * @return The absolute time since the application started.
 */
Float64 PlatformGetAbsoluteTime();

/**
 * @brief Sleep on the thread for the provided milliseconds. This blocks the main thread.
 * Should only be used for giving time back to the OS for unused update power.
 * Therefore it is not exported. Times are approximate.
 * 
 * @param Miliseconds The number of milliseconds to sleep for.
 */
void PlatformSleep(UInt64 Miliseconds);