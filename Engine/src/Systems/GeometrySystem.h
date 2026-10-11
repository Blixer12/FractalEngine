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
    UInt32 VertexCount;
    Vertex3D* Vertices;
    UInt32 IndexCount;
    UInt32* Indices;
    char Name[GeometryNameMaxLength];
    char MaterialName[MaterialNameMaxLength];
} GeometryConfig;

Bool8 GeometrySystemInitialize(UInt64* MemoryRequirement, void* State, GeometrySystemConfig Config);
void GeometrySystemShutdown(void* State);

Geometry* GeometrySystemAcquireByID(UInt32 ID);
Geometry* GeometrySystemAcquireFromConfig(GeometryConfig Config, Bool8 AutoRelease);

void GeometrySystemReleasevoid (Geometry* Geometry);

Geometry* GeometrySystemGetDefault();

GeometryConfig GeometrySystemGeneratePlaneConfig(Float32 Width, Float32 Height, UInt32 XSegmentCount, UInt32 YSegmentCount, Float32 TileX, Float32 TileY, const char* Name, const char* MaterialName);