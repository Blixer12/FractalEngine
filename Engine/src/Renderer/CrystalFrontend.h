#pragma once

#include "CrystalDef.inl"

struct PlatformState;
struct StaticMeshData;

Bool8 CrystalInitialize(const char* AppName, struct PlatformState* Platform);
void CrystalShutdown();

void CrystalOnResize(UInt16 Width, UInt16 Height);

Bool8 CrystalDrawFrame(RenderPacket* Packet);