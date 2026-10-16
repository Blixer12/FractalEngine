/**
 * @file GameInstance.h
 * @author Blixer12
 * @brief This file contains types to be consumed by the game library.
 * @version 1.0
 * @date 2026-10-11
 * 
 * @copyright Fractal Engine is Copyright (c) Blixer12 2026
 * 
 */

#pragma once

#include "Core/App.h"

/**
 * @brief Represents the basic game state in a game.
 * Called for creation by the application.
 */
typedef struct Game {
    /** @brief The application configuration. */
    AppConfig Config;

    /** 
     * @brief Function pointer to game's initialize function. 
     * @param Instance A pointer to the game instance.
     * @returns True on success; otherwise false.
     * */
    Bool8 (*Initialize)(struct Game* Instance);

    /** 
     * @brief Function pointer to game's update function. 
     * @param Instance A pointer to the game instance.
     * @param DeltaTime The time in seconds since the last frame.
     * @returns True on success; otherwise false.
     * */
    Bool8 (*Update)(struct Game* Instance, Float32 DeltaTime);

    /** 
     * @brief Function pointer to game's render function. 
     * @param Instance A pointer to the game instance.
     * @param DeltaTime The time in seconds since the last frame.
     * @returns True on success; otherwise false.
     * */
    Bool8 (*Render)(struct Game* Instance, Float32 DeltaTime);

     /** 
     * @brief Function pointer to handle resizes, if applicable. 
     * @param Instance A pointer to the game instance.
     * @param Width The width of the window in pixels.
     * @param Height The height of the window in pixels.
     * */
    void (*OnResize)(struct Game* Instance, UInt32 Width, UInt32 Height);

    /** @brief Game-specific game state. Created and managed by the game. */
    void* State;

    /** @brief The required size for the game state */
    UInt64 StateMemoryRequirement;

    /** @brief A block of memory to hold the application state. Created and managed by the engine. */
    void* AppState;
} Game;