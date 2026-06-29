#pragma once

#include "Core/App.h"

typedef struct Game {
    // The application configuration.
    AppConfig Config;

    // Function pointer to game's initialize function.
    Bool8 (*Initialize)(struct Game* Instance);

    // Function pointer to game's update function.
    Bool8 (*Update)(struct Game* Instance, Float32 DeltaTime);

    // Function pointer to game's render function.
    Bool8 (*Render)(struct Game* Instance, Float32 DeltaTime);

    // Function pointer to handle resizes, if applicable.
    void (*OnResize)(struct Game* Instance, UInt32 Width, UInt32 Height);

    // Game-specific game state. Created and managed by the game.
    void* State;
} Game;