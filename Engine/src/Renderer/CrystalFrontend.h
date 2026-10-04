#pragma once

#include "CrystalDef.inl"

struct PlatformState;
struct StaticMeshData;

Bool8 CrystalInitialize(UInt64* MemoryRequirement, void* State, const char* AppName);
void CrystalShutdown();

void CrystalOnResize(UInt16 Width, UInt16 Height);

Bool8 CrystalDrawFrame(RenderPacket* Packet);