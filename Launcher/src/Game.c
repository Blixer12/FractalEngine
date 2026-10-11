#include "Game.h"

#include <Core/Logger.h>

#include <Core/Input.h>
#include <Core/Event.h>

#include <Math/FMath.h>

// HACK: This should not be available outside the engine
#include <Renderer/CrystalFrontend.h>

void RecalculateViewMatrix(GameState* State)
{
    if (State->CameraViewUpdated)
    {
        Mat4 Rotation;
        Mat4 Translation;

        Mat4EulerXYZ(State->CameraEuler.x, State->CameraEuler.y, State->CameraEuler.z, &Rotation);
        Mat4Translation(State->CameraPosition, &Translation);

        Mat4Mul(&Rotation, &Translation, &State->View);
        Mat4Inverse(&State->View);

        State->CameraViewUpdated = false;
    }
}

void CameraPitch(GameState* State, Float32 Amount)
{
    State->CameraEuler.x += Amount;

    Float32 Limit = DegreesToRadians(89.0f);
    State->CameraEuler.x = FCLAMP(Amount, -Limit, Limit);

    State->CameraViewUpdated = true;
}

void CameraYaw(GameState* State, Float32 Amount)
{
    State->CameraEuler.y += Amount;
    State->CameraViewUpdated = true;
}

Bool8 GameInitialize(Game* Instance) 
{
    (void)Instance;
    FLDEBUG("GameInitialized() called!");

    GameState* State = (GameState*)Instance->State;

    State->CameraPosition = (Vec3){.x = 0.0f, .y = 0.0f, .z = 30.0f};
    State->CameraEuler = Vec3Zero();

    Mat4Translation(State->CameraPosition, &State->View);
    State->CameraViewUpdated = true;

    return true;
}

Bool8 GameUpdate(Game* Instance, Float32 DeltaTime) 
{
    // NOTE: Temporary
    if (InputIsKeyUp(TKey) && InputWasKeyDown(TKey))
    {
        FLDEBUG("Swapping texture!");
        EventContext Context = {0};
        EventFire(EVENT_DEBUG0, Instance, Context);
    }
    // NOTE: End temporary

    GameState* State = (GameState*)Instance->State;

    Float32 TemporaryMoveSpeed = 50.0f;
    Vec3 Velocity = Vec3Zero();

    if (InputIsKeyDown(WKey))
    {
        Vec3 Forward = Mat4Forward(&State->View);
        Velocity = Vec3Add(Velocity, Forward);
    }

    if (InputIsKeyDown(AKey))
    {
        Vec3 Left = Mat4Left(&State->View);
        Velocity = Vec3Add(Velocity, Left);
    }

    if (InputIsKeyDown(SKey))
    {
        Vec3 Backwards = Mat4Backward(&State->View);
        Velocity = Vec3Add(Velocity, Backwards);
    }

    if (InputIsKeyDown(DKey))
    {
        Vec3 Right = Mat4Right(&State->View);
        Velocity = Vec3Add(Velocity, Right);
    }

    if (InputIsKeyDown(SpaceKey))
    {
        Velocity.y += 1.0f;
    }

    if (InputIsKeyDown(XKey))
    {
        Velocity.y -= 1.0f;
    }

    if (InputIsKeyDown(QKey) || InputIsKeyDown(LeftKey))
    {
        CameraYaw(State, 3.0f * DeltaTime);
    }

    if (InputIsKeyDown(EKey) || InputIsKeyDown(RightKey))
    {
        CameraYaw(State, -3.0f * DeltaTime);
    }

    if (InputIsKeyDown(UpKey))
    {
        CameraPitch(State, 3.0f * DeltaTime);
    }

    if (InputIsKeyDown(DownKey))
    {
        CameraPitch(State, -3.0f * DeltaTime);
    }

    Vec3 Zero = Vec3Zero();
    if (!Vec3Compare(Zero, Velocity, 0.0002f))
    {
        Vec3Normalize(&Velocity);
        State->CameraPosition.x += Velocity.x * TemporaryMoveSpeed * DeltaTime;
        State->CameraPosition.y += Velocity.y * TemporaryMoveSpeed * DeltaTime;
        State->CameraPosition.z += Velocity.z * TemporaryMoveSpeed * DeltaTime;
        State->CameraViewUpdated = true;
    }


    RecalculateViewMatrix(State);

    // HACK: This should not be available outside the engine
    CrystalSetView(State->View);

    return true;
}

Bool8 GameRender(Game* Instance, Float32 DeltaTime) 
{
    (void)Instance;
    (void)DeltaTime;

    return true;
}

void GameOnResize(Game* Instance, UInt32 Width, UInt32 Height) 
{
    (void)Instance;
    (void)Width;
    (void)Height;
}