#pragma once

#include "Core/App.h"
#include "Core/Logger.h"
#include "GameInstance.h"
#include "Core/Memory.h"

extern Bool8 CreateGame(Game* TargetGame);

int main()
{

InitializeMemory();

Game Instance;
// Initalization
if (!CreateGame(&Instance))
{
    FLFATAL("Couldn't Create Game!");
    return -1;
}

 // Ensure the function pointers exist.
 if (!Instance.Render || !Instance.Update || !Instance.Initialize || !Instance.OnResize) {
     FLFATAL("The game's function pointers must be assigned!");
     return -2;
 }

if (!AppCreate(&Instance))
{
    FLFATAL("Application failed to create!");
    return 1;
}

// Begins the Game Loop
if (!AppRun())
{
    FLWARN("The Application did not shutdown properly!");
    return 2;
}

ShutdownMemory();

    return 0;
}