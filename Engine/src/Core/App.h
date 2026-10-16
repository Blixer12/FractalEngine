/**
 * @file App.h
 * @author Blixer12
 * @brief This file contains structures and logic pertaining to the
 * overall engine application itself. 
 * The application is responsible for managing both the platform layers
 * as well as all systems within the engine.
 * 
 * @version 1.0
 * @date 2026-10-11
 * 
 * @copyright Fractal Engine is Copyright (c) Blixer12 2026
 * 
 */

#pragma once

#include "Defines.h"

struct Game;

/**
 * @brief Configuration settings required for application and window initialization.
 */
typedef struct AppConfig 
{
    /** @brief Window starting position on the X axis, if applicable. */
    Int16 StartX;

    /** @brief Window starting position on the Y axis, if applicable. */
    Int16 StartY;

    /** @brief Window starting width if applicable. */
    Int16 StartWidth;

    /** @brief Window starting height if applicable. */
    Int16 StartHeight;

    /** @brief The application name used in windowing, if applicable. */
    char* Name;
} AppConfig;

/**
 * @brief Creates the application, standing up the platform layer and all
 * underlying subsystems.
 * @param Instance A pointer to the game instance associated with the application
 * @returns True on success; otherwise false.
 */
FAPI Bool8 AppCreate(struct Game* Instance);

/**
 * @brief Starts the main application loop.
 * @returns True on success; otherwise false.
 */
FAPI Bool8 AppRun();

/**
 * @brief Obtains the framebuffer size of the application.
 * @deprecated NOTE: This is temporary, and should be removed once FVars are in place.
 */
void AppGetWindowSize(UInt32* Width, UInt32* Height);