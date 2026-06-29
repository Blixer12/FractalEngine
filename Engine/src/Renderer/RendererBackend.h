#pragma once

#include "RenderDef.inl"

struct PlatformState;

Bool8 RendererBackendCreate(RendererBackendType Type, struct PlatformState* Platform, RendererBackend* Backend);
void RendererBackendDestroy(RendererBackend* Backend);