#pragma once

#include "Defines.h"

struct Game;

// Application configuration.
typedef struct AppConfig 
{
    // Window starting position x axis, if applicable.
    Int16 StartX;

    // Window starting position y axis, if applicable.
    Int16 StartY;

    // Window starting width, if applicable.
    Int16 StartWidth;

    // Window starting height, if applicable.
    Int16 StartHeight;

    // The application name used in windowing, if applicable.
    char* Name;
} AppConfig;

FAPI Bool8 AppCreate(struct Game* Instance);

FAPI Bool8 AppRun();