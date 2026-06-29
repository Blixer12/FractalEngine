#pragma once

#include <Defines.h>
#include <GameInstance.h>

typedef struct GameState {
    Float32 DeltaTime;
} GameState;

Bool8 GameInitialize(Game* Instance);

Bool8 GameUpdate(Game* Instance, Float32 DeltaTime);

Bool8 GameRender(Game* Instance, Float32 DeltaTime);

void GameOnResize(Game* Instance, UInt32 Width, UInt32 Height);