/**
 * @file GeometrySystem.h
 * @author Blixer12
 * @brief The geometry system is responsible for loading and unloading geometry,
 * as well as reference counting.
 * 
 * @version 1.0
 * @date 2026-10-11
 * 
 * @copyright Fractal Engine is Copyright (c) Blixer12 2026
 * 
 */

#pragma once

#include "Renderer/CrystalDef.inl"

/** @brief The geometry system configuration. */
typedef struct GeometrySystemConfig {
    /**
     * @brief NOTE: Should be significantly greater than the number of static meshes because
     * the there can and will be more than one of these per mesh.
     * Take other systems into account as well.
     */
    UInt32 MaxGeometryCount;
} GeometrySystemConfig;

/** @brief The name of the default geometry. */
constexpr char DefaultGeometryName[] = "Default";

/**
 * @brief Represents the configuration for a geometry.
 */
typedef struct GeometryConfig {
    /** @brief The size of each vertex. */
    UInt32 VertexSize;
    /** @brief The number of vertices. */
    UInt32 VertexCount;
    /** @brief an array of vertices. */
    void* Vertices;
    /** @brief The size of each index. */
    UInt32 IndexSize;
    /** @brief The number of indices. */
    UInt32 IndexCount;
    /** @brief An array of indices. */
    void* Indices;
    /** @brief The name of the geometry. */
    char Name[GeometryNameMaxLength];
    /** @brief The name of the material used by the geometry. */
    char MaterialName[MaterialNameMaxLength];
} GeometryConfig;

/**
 * @brief Initializes the geometry system.
 * Should be called twice; once to get the memory requirement (passing state=0), and a second
 * time passing an allocated block of memory to actually initialize the system.
 * 
 * @param MemoryRequirement A pointer to hold the memory requirement as it is calculated.
 * @param State A block of memory to hold the state or, if gathering the memory requirement, 0.
 * @param Config The configuration for this system.
 * @return True on success; otherwise false.
 */
Bool8 GeometrySystemInitialize(UInt64* MemoryRequirement, void* State, GeometrySystemConfig Config);

/**
 * @brief Shuts down the geometry system.
 * 
 * @param State The state block of memory.
 */
void GeometrySystemShutdown(void* State);

/**
 * @brief Acquires an existing geometry by id.
 * 
 * @param IS The geometry identifier to acquire by.
 * @return A pointer to the acquired geometry or nullptr if failed.
 */
Geometry* GeometrySystemAcquireByID(UInt32 ID);

/**
 * @brief Registers and acquires a new geometry using the given config.
 * 
 * @param Config The geometry configuration.
 * @param AutoRelease Indicates if the acquired geometry should be unloaded when its reference count reaches 0.
 * @return A pointer to the acquired geometry or nullptr if failed. 
 */
Geometry* GeometrySystemAcquireFromConfig(GeometryConfig Config, Bool8 AutoRelease);

/**
 * @brief Releases a reference to the provided geometry.
 * 
 * @param Geometry The geometry to be released.
 */
void GeometrySystemRelease (Geometry* Geometry);

/**
 * @brief Obtains a pointer to the default geometry.
 * 
 * @return A pointer to the default geometry. 
 */
Geometry* GeometrySystemGetDefault();

/**
 * @brief Obtains a pointer to the default geometry.
 * 
 * @return A pointer to the default geometry. 
 */
Geometry* GeometrySystemGetDefault2D();

/**
 * @brief Generates configuration for plane geometries given the provided parameters.
 * NOTE: vertex and index arrays are dynamically allocated and should be freed upon object disposal.
 * Thus, this should not be considered production code.
 * 
 * @param Width The overall width of the plane. Must be non-zero.
 * @param Height The overall height of the plane. Must be non-zero.
 * @param XSegmentCount The number of segments along the x-axis in the plane. Must be non-zero.
 * @param YSegmentCount The number of segments along the y-axis in the plane. Must be non-zero.
 * @param TileX The number of times the texture should tile across the plane on the x-axis. Must be non-zero.
 * @param TileY The number of times the texture should tile across the plane on the y-axis. Must be non-zero.
 * @param Name The name of the generated geometry.
 * @param MaterialName The name of the material to be used.
 * @return A geometry configuration which can then be fed into geometry_system_acquire_from_config().
 */
GeometryConfig GeometrySystemGeneratePlaneConfig(Float32 Width, Float32 Height, UInt32 XSegmentCount, UInt32 YSegmentCount, Float32 TileX, Float32 TileY, const char* Name, const char* MaterialName);