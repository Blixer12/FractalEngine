#include "Game.h"

#include <EntryPoint.h>

//TODO: Remove This later on
#include <Core/Memory.h>

// Function to Create the Game
Bool8 CreateGame(Game* TargetGame)
{
    TargetGame->Config.StartX = 100;
    TargetGame->Config.StartY = 100;
    TargetGame->Config.StartWidth = 1280;
    TargetGame->Config.StartHeight = 720;
    TargetGame->Config.Name = "Fractal Engine Launcher";
    TargetGame->Initialize = GameInitialize;
    TargetGame->Update = GameUpdate;
    TargetGame->Render = GameRender;
    TargetGame->OnResize = GameOnResize;

    TargetGame->State = FMAllocate(sizeof(GameState), MEMORY_TAG_GAME);

    return true;
}