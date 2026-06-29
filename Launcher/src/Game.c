#include "Game.h"

#include <Core/Logger.h>

Bool8 GameInitialize(Game* Instance) 
{
    (void)Instance;
    FLDEBUG("GameInitialized() called!");
    return TRUE;
}

Bool8 GameUpdate(Game* Instance, Float32 DeltaTime) 
{
    (void)Instance;
    (void)DeltaTime;
    return TRUE;
}

Bool8 GameRender(Game* Instance, Float32 DeltaTime) 
{
    (void)Instance;
    (void)DeltaTime;
    return TRUE;
}

void GameOnResize(Game* Instance, UInt32 Width, UInt32 Height) 
{
    (void)Instance;
    (void)Width;
    (void)Height;
}