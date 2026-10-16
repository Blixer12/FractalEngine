/**
 * @file EntryPoint.h
 * @author Blixer12
 * @brief This file contains the main entry point to the application. 
 * It also contains a reference to an externally defined CreateGame
 * method, which should create and set a custom game object to the
 * location pointed to by TargetGame. This would be provided by the
 * consuming application, which is then hooked into the engine itself
 * during the bootstrapping phase.
 * @version 1.0
 * @date 2026-10-11
 * 
 * @copyright Fractal Engine is Copyright (c) Blixer12 2026
 * 
 */

#pragma once

#include "Core/App.h"
#include "Core/Logger.h"
#include "GameInstance.h"

/** @brief Externally-defined function to create a game, provided by the consumer
 * of this library.
 * @param TargetGame A pointer which holds the created game object as provided by the consumer.
 * @returns True on successful creation; otherwise false.
 */
extern Bool8 CreateGame(Game* TargetGame);

int main()
{

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
    return -3;
}

// Begins the Game Loop
if (!AppRun())
{
    FLWARN("The Application did not shutdown properly!");
    return -4;
}

    return 0;
}