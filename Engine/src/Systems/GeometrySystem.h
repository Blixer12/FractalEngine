#pragma once

#include "Renderer/CrystalDef.inl"

typedef struct GeometrySystemConfig {
    // Probably needs more fields...
    // Also needs to be A lot, significantly greater than amt of static meshes, because 
    // There can AND will be more than 1 geometry per mesh!
    UInt32 MaxGeometryCount;
} GeometrySystemConfig;

constexpr char DefaultGeometryName[] = "Default";

typedef struct GeometryConfig {
    UInt32 VertexSize;
    UInt32 VertexCount;
    void* Vertices;
    UInt32 IndexSize;
    UInt32 IndexCount;
    void* Indices;
    char Name[GeometryNameMaxLength];
    char MaterialName[MaterialNameMaxLength];
} GeometryConfig;

Bool8 GeometrySystemInitialize(UInt64* MemoryRequirement, void* State, GeometrySystemConfig Config);
void GeometrySystemShutdown(void* State);

Geometry* GeometrySystemAcquireByID(UInt32 ID);
Geometry* GeometrySystemAcquireFromConfig(GeometryConfig Config, Bool8 AutoRelease);

void GeometrySystemReleasevoid (Geometry* Geometry);

Geometry* GeometrySystemGetDefault();

Geometry* GeometrySystemGetDefault2D();

GeometryConfig GeometrySystemGeneratePlaneConfig(Float32 Width, Float32 Height, UInt32 XSegmentCount, UInt32 YSegmentCount, Float32 TileX, Float32 TileY, const char* Name, const char* MaterialName);