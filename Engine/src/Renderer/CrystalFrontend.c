#include "CrystalFrontend.h"

#include "CrystalBackend.h"

#include "Core/Logger.h"
#include "Core/Memory.h"
#include "Math/FMath.h"

#include "Resources/ResourceDef.h"

#include "Systems/TextureSystem.h"
#include "Systems/MaterialSystem.h"

// NOTE: Temporary
#include "Core/FString.h"
#include "Core/Event.h"
// NOTE: End temporary

typedef struct CrystalState {
    CrystalBackend Backend;
    Mat4 Projection;
    Mat4 View;
    Float32 NearClip;
    Float32 FarClip;

    // NOTE: Temporary
    Material* TestMaterial;
    // NOTE: End temporary
} CrystalState;

static CrystalState* StatePtr;

Bool8 EventOnDebugEvent(UInt16 Code, void* Sender, void* Reciever, EventContext Data)
{
    (void)Code;
    (void)Sender;
    (void)Reciever;
    (void)Data;

    const char* Names[4] = {
        "Cobblestone",
        "Paving",
        "Paving2",
        "WhiteStone"};
    static Int8 Choice = 3;

    const char* OldName = Names[Choice];

    Choice++;
    Choice %= 4;

    // Acquire the new texture
    StatePtr->TestMaterial->BaseColorMap.Texture = TextureSystemAcquire(Names[Choice], true);
    if (!StatePtr->TestMaterial->BaseColorMap.Texture)
    {
        FLWARN("EventOnDebugEvent - No Texture! using default!");
        StatePtr->TestMaterial->BaseColorMap.Texture = TextureSystemGetDefaultTexture();
    }

    TextureSystemRelease(OldName);
    return true;
}

// NOTE: End temporary

Bool8 CrystalInitialize(UInt64* MemoryRequirement, void* State, const char* AppName)
{
    *MemoryRequirement = sizeof(CrystalState);
    if (State == 0) {
        return true;
    }
    StatePtr = State;

    // NOTE: Temporary
    EventRegister(EVENT_DEBUG0, StatePtr, EventOnDebugEvent);
    // NOTE: End temporary

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
    StatePtr->Projection = Mat4Perspective(DegreesToRadians(45.0f), 1280/720.0f, StatePtr->NearClip, StatePtr->FarClip);

    Vec3 Position = {.x = 0.0f, .y =0.0f, .z = 30.0f}; // 30.0f
    StatePtr->View = Mat4Translation(Position);
    StatePtr->View = Mat4Inverse(StatePtr->View);

    return true;
}
void CrystalShutdown()
{
    if (StatePtr) {
        // NOTE: Temporary
        EventUnregister(EVENT_DEBUG0, StatePtr, EventOnDebugEvent);
        // NOTE: End temporary

        StatePtr->Backend.Shutdown(&StatePtr->Backend);
    }
    

    StatePtr = 0;
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

        Mat4 Model = Mat4Translation((Vec3){.x = 0.0f, .y = 0.0f, .z = 0.0f});
        // static Float32 Angle = 0.0f;
        // Angle += 0.03f;
        // Quaternion Rotation = QuaternionFromAxisAngle(Vec3Forward(), Angle, false);
        // Mat4 Model = QuaternionToRotationMatrix(Rotation, Vec3Zero());
        GeometryRenderData Data = {0};
        Data.Model = Model;

        // TODO: Temporary
        // Grab default if it aint real
        if (!StatePtr->TestMaterial)
        {
            // Auto Config
            StatePtr->TestMaterial = MaterialSystemAcquire("TestMaterial");
            if (!StatePtr->TestMaterial)
            {
                FLWARN("Auto material load failed, falling back to default texture");

                MaterialConfig Config = {0};
                StringNcopy(Config.Name, "TestMaterial", MaterialNameMaxLength);
                Config.AutoRelease = false;
                Config.BaseColor = Vec4One();
                StringNcopy(Config.BaseColorMapName, DefaultTextureName, TextureNameMaxLength);
                StatePtr->TestMaterial = MaterialSystemAcquireFromConfig(Config);
            }
        }

        Data.Material = StatePtr->TestMaterial;
        StatePtr->Backend.UpdateObject(&Data);

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

void CrystalCreateTexture(const UInt8* Pixels, struct Texture* Texture)
{
    StatePtr->Backend.CreateTexture(Pixels, Texture);
}
        
void CrystalDestroyTexture(struct Texture* Texture)
{
    StatePtr->Backend.DestroyTexture(Texture);
}

Bool8 CrystalCreateMaterial(struct Material* Material) 
{
    return StatePtr->Backend.CreateMaterial(Material);
}

void CrystalDestroyMaterial(struct Material* Material) 
{
    StatePtr->Backend.DestroyMaterial(Material);
}