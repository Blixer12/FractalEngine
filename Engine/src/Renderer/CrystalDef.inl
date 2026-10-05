#pragma once

#include "Defines.h"
#include "Math/MathDef.h"

typedef enum CrystalBackckendType {
    // Modern APIs
    CRYSTAL_BACKEND_TYPE_VULKAN,
    CRYSTAL_BACKEND_TYPE_DIRECTX12,
    CRYSTAL_BACKEND_TYPE_METAL,

    // Legacy APIs
    CRYSTAL_BACKEND_TYPE_OPENGL,
    CRYSTAL_BACKEND_TYPE_DIRECTX11
} CrystalBackendType;

typedef struct GlobalUniformObject {
    Mat4 Projection;
    Mat4 View;
    Mat4 Model;
    Mat4 MatrixReserved0;
}GlobalUniformObject;

typedef struct CrystalBackend {
    UInt64 FrameNumber;

    Bool8 (*Initialize)(struct CrystalBackend* Backend, const char* AppName);
    void (*Shutdown)(struct CrystalBackend* Backend);

    void (*Resized)(struct CrystalBackend* Backend, UInt16 Width, UInt16 Height);

    Bool8 (*BeginFrame)(struct CrystalBackend* Backend, Float32 DeltaTime);
    void (*UpdateGlobalState)(Mat4 Projection, Mat4 View, Vec3 ViewPosition, Vec4 AmbientColor, Int32 Mode);
    Bool8 (*EndFrame)(struct CrystalBackend* Backend, Float32 DeltaTime);

    void (*UpdateObject)(Mat4 Model);
} CrystalBackend;

typedef struct RenderPacket {
    Float32 DeltaTime;
} RenderPacket;