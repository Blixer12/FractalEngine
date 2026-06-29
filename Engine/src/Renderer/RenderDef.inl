#pragma once

#include "Defines.h"

typedef enum RendererBackendType {
    RENDERER_BACKEND_TYPE_VULKAN,
    RENDERER_BACKEND_TYPE_OPENGL,
    RENDERER_BACKEND_TYPE_DIRECTX
} RendererBackendType;

typedef struct RendererBackend {
    struct PlatformState* Platform;
    UInt64 FrameNumber;

    Bool8 (*Initialize)(struct RendererBackend* Backend, const char* AppName, struct PlatformState* Platform);
    void (*Shutdown)(struct RendererBackend* Backend);

    void (*Resized)(struct RendererBackend* Backend, UInt16 Width, UInt16 Height);

    Bool8 (*BeginFrame)(struct RendererBackend* Backend, Float32 DeltaTime);
    Bool8 (*EndFrame)(struct RendererBackend* Backend, Float32 DeltaTime);    
} RendererBackend;

typedef struct RenderPacket {
    Float32 DeltaTime;
} RenderPacket;