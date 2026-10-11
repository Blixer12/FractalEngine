#include "CrystalFrontend.h"

#include "CrystalBackend.h"

#include "Core/Logger.h"
#include "Core/Memory.h"
#include "Math/FMath.h"

#include "Resources/ResourceDef.h"

#include "Systems/TextureSystem.h"
#include "Systems/MaterialSystem.h"

typedef struct CrystalState {
    CrystalBackend Backend;
    Mat4 Projection;
    Mat4 View;
    Mat4 UIProjection;
    Mat4 UIView;
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

    // World projection/view
    StatePtr->NearClip = 0.001f;
    StatePtr->FarClip = 1000.0f;
    Mat4Perspective(DegreesToRadians(45.0f), 1280/720.0f, StatePtr->NearClip, StatePtr->FarClip, &StatePtr->Projection);

    // configurable camera starting position maybe
    Vec3 Position = {.x = 0.0f, .y =0.0f, .z = 30.0f}; // 30.0f
    Mat4Translation(Position, &StatePtr->View);
    Mat4Inverse(&StatePtr->View);

    // UI Projection
    Mat4Orthographic(0, 1280.0f, 720.0f, 0, -100.0f, 100.0f, &StatePtr->UIProjection);
    Mat4Inverse(Mat4Identity(&StatePtr->UIView));

    return true;
}
void CrystalShutdown()
{
    if (StatePtr) {

        StatePtr->Backend.Shutdown(&StatePtr->Backend);
    }
    

    StatePtr = 0;
}

void CrystalOnResize(UInt16 Width, UInt16 Height)
{
    if (Width == 0 || Height == 0) {
        return;
    }

    if (StatePtr) {
        Mat4Perspective(DegreesToRadians(45.0f), Width/(Float32)Height, StatePtr->NearClip, StatePtr->FarClip, &StatePtr->Projection);
        Mat4Orthographic(0, (Float32)Width, (Float32)Height, 0, -100.0f, 100.0f, &StatePtr->UIProjection);
        StatePtr->Backend.Resized(&StatePtr->Backend, Width, Height);
    } else {
        FLERROR("The Crystal backend does not exist to accept resize: %i, %i", Width, Height);
    }
}

Bool8 CrystalDrawFrame(RenderPacket* Packet)
{
    if (StatePtr->Backend.BeginFrame(&StatePtr->Backend, Packet->DeltaTime))
    {
        if (!StatePtr->Backend.BeginRenderpass(&StatePtr->Backend, BUILTIN_RENDERPASS_WORLD))
        {
            FLERROR("Backend.BeginRenderpass -> BUILTIN_RENDERPASS_WORLD failed. app shutting down");
            return false;
        }

        StatePtr->Backend.UpdateGlobalWorldState(StatePtr->Projection, StatePtr->View, Vec3Zero(), Vec4One(), 0);

        UInt32 Count = Packet->GeometryCount;
        for (UInt32 i = 0; i < Count; ++i)
        {
            StatePtr->Backend.DrawGeometry(Packet->Geometries[i]);
        }

        if (!StatePtr->Backend.EndRenderpass(&StatePtr->Backend, BUILTIN_RENDERPASS_WORLD))
        {
            FLERROR("Backend.BeginRenderpass -> BUILTIN_RENDERPASS_WORLD failed. app shutting down");
            return false;
        }


        
        if (!StatePtr->Backend.BeginRenderpass(&StatePtr->Backend, BUILTIN_RENDERPASS_UI))
        {
            FLERROR("Backend.BeginRenderpass -> BUILTIN_RENDERPASS_UI failed. app shutting down");
            return false;
        }

        StatePtr->Backend.UpdateGlobalUIState(StatePtr->UIProjection, StatePtr->UIView, 0);

        Count = Packet->UIGeometryCount;
        for (UInt32 i = 0; i < Count; ++i)
        {
            StatePtr->Backend.DrawGeometry(Packet->UIGeometries[i]);
        }

        if (!StatePtr->Backend.EndRenderpass(&StatePtr->Backend, BUILTIN_RENDERPASS_UI))
        {
            FLERROR("Backend.BeginRenderpass -> BUILTIN_RENDERPASS_UI failed. app shutting down");
            return false;
        }

        Bool8 Result = StatePtr->Backend.EndFrame(&StatePtr->Backend, Packet->DeltaTime);

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
    StatePtr->Backend.CreateTexture(Texture, Pixels);
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

Bool8 CrystalCreateGeometry(struct Geometry* Geometry, UInt32 VertexCount, const Vertex3D* Vertices, UInt32 IndexCount, const UInt32* Indices) 
{
    return StatePtr->Backend.CreateGeometry(Geometry, VertexCount, Vertices, IndexCount, Indices);
}

void CrystalDestroyGeometry(struct Geometry* Geometry) 
{
    StatePtr->Backend.DestroyGeometry(Geometry);
}