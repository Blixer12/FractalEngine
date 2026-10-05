#include "CrystalFrontend.h"

#include "CrystalBackend.h"

#include "Core/Logger.h"
#include "Core/Memory.h"

#include "Math/FMath.h"

typedef struct CrystalState {
    CrystalBackend Backend;
    Mat4 Projection;
    Mat4 View;
    Float32 NearClip;
    Float32 FarClip;
} CrystalState;

static CrystalState* StatePtr;

Bool8 CrystalInitialize(UInt64* MemoryRequirement, void* State, const char* AppName)
{
    *MemoryRequirement = sizeof(CrystalState);
    if (State == 0) {
        return true;
    }
    StatePtr = State;

    //TODO: Make this Configurable
    CrystalBackendCreate(CRYSTAL_BACKEND_TYPE_VULKAN, &StatePtr->Backend);
    StatePtr->Backend.FrameNumber = 0;

    if (!StatePtr->Backend.Initialize(&StatePtr->Backend, AppName))
    {
        FLFATAL("Renderer Backend failed to Initialize! Shutting down Application");
        return false;
    }

    StatePtr->NearClip = 0.001f;
    StatePtr->FarClip = 1000.0f;
    StatePtr->Projection = Mat4Perspective(DegreesToRadians(70.0f), 1280/720.0f, StatePtr->NearClip, StatePtr->FarClip);

    Vec3 Position = {.x = 0.0f, .y =0.0f, .z = 30.0f}; // 30.0f
    StatePtr->View = Mat4Translation(Position);
    StatePtr->View = Mat4Inverse(StatePtr->View);

    return true;
}
void CrystalShutdown()
{
    if (StatePtr) {
        StatePtr->Backend.Shutdown(&StatePtr->Backend);
        CrystalBackendDestroy(&StatePtr->Backend);
        StatePtr = 0;
    }
}

Bool8 CrystalBeginFrame(Float32 DeltaTime)
{
    return StatePtr->Backend.BeginFrame(&StatePtr->Backend, DeltaTime);
}

Bool8 CrystalEndFrame(Float32 DeltaTime)
{
    Bool8 Result = StatePtr->Backend.EndFrame(&StatePtr->Backend, DeltaTime);
    StatePtr->Backend.FrameNumber++;
    return Result;
}

void CrystalOnResize(UInt16 Width, UInt16 Height)
{
    if (StatePtr) {
        StatePtr->Projection = Mat4Perspective(DegreesToRadians(70.0f), Width/(Float32)Height, StatePtr->NearClip, StatePtr->FarClip);
        StatePtr->Backend.Resized(&StatePtr->Backend, Width, Height);
    } else {
        FLERROR("The Crystal backend does not exist to accept resize: %i, %i", Width, Height);
    }
}

Bool8 CrystalDrawFrame(RenderPacket* Packet)
{
    if (CrystalBeginFrame(Packet->DeltaTime))
    {
        StatePtr->Backend.UpdateGlobalState(StatePtr->Projection, StatePtr->View, Vec3Zero(), Vec4One(), 0);

        // Mat4 Model = Mat4Translation((Vec3){.x = 0.0f, .y = 0.0f, .z = 0.0f});
        static Float32 Angle = 0.01f;
        Angle += 0.03f;
        Quaternion Rotation = QuaternionFromAxisAngle(Vec3Forward(), Angle, false);
        Mat4 Model = QuaternionToRotationMatrix(Rotation, Vec3Zero());
        StatePtr->Backend.UpdateObject(Model);

        Bool8 Result = CrystalEndFrame(Packet->DeltaTime);

        if (!Result)
        {
            FLERROR("CrystalEndFrame Failed! Shutting down Application");
        }
    }

    return true;
}

void CrystalSetView(Mat4 View)
{
    StatePtr->View = View;
}