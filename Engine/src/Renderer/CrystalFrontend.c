#include "CrystalFrontend.h"

#include "CrystalBackend.h"

#include "Core/Logger.h"
#include "Core/Memory.h"
#include "Math/FMath.h"

#include "Resources/ResourceDef.h"

typedef struct CrystalState {
    CrystalBackend Backend;
    Mat4 Projection;
    Mat4 View;
    Float32 NearClip;
    Float32 FarClip;

    Texture DefaultTexture;
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

    // NOTE: Creates a default texture, 256x256 checkerboard on the fly!
    // Eliminates Asset Dependency!
    
    FLDEBUG("Creating Default Texture");
    constexpr UInt32 TextureDimensions = 256;
    constexpr UInt32 Channels = 4;
    constexpr UInt32 PixelCount = TextureDimensions * TextureDimensions;
    UInt8 Pixels[PixelCount * Channels];

    FMSetMemory(Pixels, 255, sizeof(UInt8) * PixelCount * Channels);


    for (UInt64 Row = 0; Row < TextureDimensions; Row++)
    {
        for (UInt64 Column = 0; Column < TextureDimensions; Column++)
        {
            UInt64 Index = (Row * TextureDimensions) + Column;
            UInt64 IndexChannels = Index * Channels;
            if (Row % 2) {
                if (Column % 2) {
                    Pixels[IndexChannels + 0] = 0;
                    Pixels[IndexChannels + 1] = 0;
                    Pixels[IndexChannels + 1] = 255;
                }
            } else {
                if (!(Column % 2)) {
                    Pixels[IndexChannels + 0] = 0;
                    Pixels[IndexChannels + 1] = 0;
                    Pixels[IndexChannels + 2] = 255;
                }
            }
        } 
    }
    
    StatePtr->Backend.CreateTexture(
        "Default",
        false,
        TextureDimensions,
        TextureDimensions,
        4,
        Pixels,
        false,
        &StatePtr->DefaultTexture
    );

    return true;
}
void CrystalShutdown()
{
    if (StatePtr) {
        CrystalDestroyTexture(&StatePtr->DefaultTexture);
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
        Data.ObjectID = 0; // TODO: Actual Object ID
        Data.Model = Model;
        Data.Textures[0] = &StatePtr->DefaultTexture;
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

void CrystalCreateTexture(
        const char* Name,
        Bool8 AutoRelease,
        Int32 Width,
        Int32 Height,
        Int32 ChannelCount,
        const UInt8* Pixels,
        Bool8 HasTransparency,
        struct Texture* Texture) {
            StatePtr->Backend.CreateTexture(Name, AutoRelease, Width, Height, ChannelCount, Pixels, HasTransparency, Texture);
        }
        
void CrystalDestroyTexture(struct Texture* Texture)
{
    StatePtr->Backend.DestroyTexture(Texture);
}