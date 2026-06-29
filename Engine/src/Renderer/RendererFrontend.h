#pragma once

#include "RenderDef.inl"

struct PlatformState;
struct StaticMeshData;

Bool8 RendererInitialize(const char* AppName, struct PlatformState* Platform);
void RendererShutdown();

void RendererOnResize(UInt16 Width, UInt16 Height);

Bool8 RendererDrawFrame(RenderPacket* Packet);