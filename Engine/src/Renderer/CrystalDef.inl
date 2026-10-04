#pragma once

#include "Defines.h"

typedef enum CrystalBackckendType {
    // Modern APIs
    CRYSTAL_BACKEND_TYPE_VULKAN,
    CRYSTAL_BACKEND_TYPE_DIRECTX12,
    CRYSTAL_BACKEND_TYPE_METAL,

    // Legacy APIs
    CRYSTAL_BACKEND_TYPE_OPENGL,
    CRYSTAL_BACKEND_TYPE_DIRECTX11
} CrystalBackendType;

typedef struct CrystalBackend {
    struct PlatformState* Platform;
    UInt64 FrameNumber;

    Bool8 (*Initialize)(struct CrystalBackend* Backend, const char* AppName);
    void (*Shutdown)(struct CrystalBackend* Backend);

    void (*Resized)(struct CrystalBackend* Backend, UInt16 Width, UInt16 Height);

    Bool8 (*BeginFrame)(struct CrystalBackend* Backend, Float32 DeltaTime);
    Bool8 (*EndFrame)(struct CrystalBackend* Backend, Float32 DeltaTime);    
} CrystalBackend;

typedef struct RenderPacket {
    Float32 DeltaTime;
} RenderPacket;