#pragma once

#include "Defines.h"
#include "Math/MathDef.h"
#include "Resources/ResourceDef.h"

typedef enum CrystalBackckendType {
    // Modern APIs
    CRYSTAL_BACKEND_TYPE_VULKAN,
    CRYSTAL_BACKEND_TYPE_DIRECTX12,
    CRYSTAL_BACKEND_TYPE_METAL,

    // Legacy APIs
    CRYSTAL_BACKEND_TYPE_OPENGL,
    CRYSTAL_BACKEND_TYPE_DIRECTX11
} CrystalBackendType;

typedef struct GeometryRenderData {
    Mat4 Model;
    Geometry* Geometry;
} GeometryRenderData;

typedef enum BuiltinRenderpass {
    BUILTIN_RENDERPASS_WORLD = 0x01,
    BUILTIN_RENDERPASS_UI = 0x02
} BuiltinRenderpass; 

typedef struct CrystalBackend {
    UInt64 FrameNumber;

    Bool8 (*Initialize)(struct CrystalBackend* Backend, const char* AppName);
    void (*Shutdown)(struct CrystalBackend* Backend);

    void (*Resized)(struct CrystalBackend* Backend, UInt16 Width, UInt16 Height);

    Bool8 (*BeginFrame)(struct CrystalBackend* Backend, Float32 DeltaTime);
    Bool8 (*EndFrame)(struct CrystalBackend* Backend, Float32 DeltaTime);

    void (*UpdateGlobalWorldState)(Mat4 Projection, Mat4 View, Vec3 ViewPosition, Vec4 AmbientColor, Int32 Mode);
    void (*UpdateGlobalUIState)(Mat4 Projection, Mat4 View, Int32 Mode);

    Bool8 (*BeginRenderpass)(struct CrystalBackend* Backend, UInt8 RenderpassID);
    Bool8 (*EndRenderpass)(struct CrystalBackend* Backend, UInt8 RenderpassID);

    void (*DrawGeometry)(GeometryRenderData Data);

    void (*CreateTexture)(struct Texture* Texture, const UInt8* Pixels);
    void (*DestroyTexture)(struct Texture* Texture);

    Bool8 (*CreateMaterial)(struct Material* Material);
    void (*DestroyMaterial)(struct Material* Material);

    Bool8 (*CreateGeometry)(struct Geometry* Geometry, UInt32 VertexCount, const Vertex3D* Vertices, UInt32 IndexCount, const UInt32* Indices);
    void (*DestroyGeometry)(struct Geometry* Geometry);
} CrystalBackend;

typedef struct RenderPacket {
    Float32 DeltaTime;

    UInt32 GeometryCount;
    GeometryRenderData* Geometries;

    UInt32 UIGeometryCount;
    GeometryRenderData* UIGeometries;
} RenderPacket;