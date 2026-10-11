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

void CrystalCreateTexture(const UInt8* Pixels, struct Texture* Texture);
        
void CrystalDestroyTexture(struct Texture* Texture);

Bool8 CrystalCreateMaterial(struct Material* Material);
void CrystalDestroyMaterial(struct Material* Material);

Bool8 CrystalCreateGeometry(struct Geometry* Geometry, UInt32 VertexSize, UInt32 VertexCount, const void* Vertices, UInt32 IndexSize, UInt32 IndexCount, const void* Indices);
void CrystalDestroyGeometry(struct Geometry* Geometry);