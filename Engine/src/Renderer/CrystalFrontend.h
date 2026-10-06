#pragma once

#include "CrystalDef.inl"

struct PlatformState;
struct StaticMeshData;

Bool8 CrystalInitialize(UInt64* MemoryRequirement, void* State, const char* AppName);
void CrystalShutdown();

void CrystalOnResize(UInt16 Width, UInt16 Height);

Bool8 CrystalDrawFrame(RenderPacket* Packet);

// HACK: should not be exposed...
FAPI void CrystalSetView(Mat4 View);

void CrystalCreateTexture(
        const char* Name,
        Bool8 AutoRelease,
        Int32 Width,
        Int32 Height,
        Int32 ChannelCount,
        const UInt8* Pixels,
        Bool8 HasTransparency,
        struct Texture* Texture);
        
void CrystalDestroyTexture(struct Texture* Texture);