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


/**
 * @brief A generic "interface" for the backend. Crystal backend
 * is what is responsible for making calls to the graphics API such as
 * Vulkan, OpenGL or DirectX. Each of these should implement this interface.
 * The frontend only interacts via this structure and has no knowledge of
 * the way things actually work on the backend.
 */
typedef struct CrystalBackend {
    UInt64 FrameNumber;

    /**
     * @brief Initializes the backend.
     * 
     * @param Backend A pointer to the generic backend interface.
     * @param AppName The name of the application.
     * @return True if initialized successfully; otherwise false.
     */
    Bool8 (*Initialize)(struct CrystalBackend* Backend, const char* AppName);

    /**
     * @brief Shuts Crystal backend down.
     * 
     * @param Backend A pointer to the generic backend interface.
     */
    void (*Shutdown)(struct CrystalBackend* Backend);

    /**
     * @brief Handles window resizes.
     * 
     * @param Backend A pointer to the generic backend interface.
     * @param Width The new window width.
     * @param Height The new window height.
     */
    void (*Resized)(struct CrystalBackend* Backend, UInt16 Width, UInt16 Height);

    /**
     * @brief Performs setup routines required at the start of a frame.
     * @note A false result does not necessarily indicate failure. It can also specify that
     * the backend is simply not in a state capable of drawing a frame at the moment, and 
     * that it should be attempted again on the next loop. End frame does not need to (and
     * should not) be called if this is the case.
     * @param Backend A pointer to the generic backend interface.
     * @param DeltaTime The time in seconds since the last frame.
     * @return True if successful; otherwise false. 
     */
    Bool8 (*BeginFrame)(struct CrystalBackend* Backend, Float32 DeltaTime);

    /**
     * @brief Performs routines required to draw a frame, such as presentation. Should only be called
     * after a successful return of begin_frame.
     * 
     * @param Backend A pointer to the generic backend interface.
     * @param DeltaTime The time in seconds since the last frame.
     * @return True on success; otherwise false.
     */
    Bool8 (*EndFrame)(struct CrystalBackend* Backend, Float32 DeltaTime);


    /**
     * @brief Updates global state items, such as view and projection. Should only be called
     * while in the world renderpass.
     * 
     * @param Projection The projection matrix to be set.
     * @param View The view matrix to be set.
     * @param ViewPosition The view position (camera position) to be set.
     * @param AmbientColor THe ambient world color.
     * @param Mode The render mode.
     */
    void (*UpdateGlobalWorldState)(Mat4 Projection, Mat4 View, Vec3 ViewPosition, Vec4 AmbientColor, Int32 Mode);

    /**
     * @brief Updates global state items for the UI, such as view and projection. Should only be
     * called while in the UI renderpass.
     * 
     * @param Projection The projection matrix to be set.
     * @param View The view matrix to be set.
     * @param Mode The render mode.
     */
    void (*UpdateGlobalUIState)(Mat4 Projection, Mat4 View, Int32 Mode);

    /**
     * @brief Begins a renderpass with the given id.
     * 
     * @param Backend A pointer to the generic backend interface.
     * @param RenderpassID The identifier of the renderpass to begin.
     * @return True on success; otherwise false.
     */
    Bool8 (*BeginRenderpass)(struct CrystalBackend* Backend, UInt8 RenderpassID);

    /**
     * @brief Ends a renderpass with the given id.
     * 
     * @param Backend A pointer to the generic backend interface.
     * @param RenderpassID The identifier of the renderpass to end.
     * @return True on success; otherwise false.
     */
    Bool8 (*EndRenderpass)(struct CrystalBackend* Backend, UInt8 RenderpassID);


    /**
     * @brief Draws the given geometry. Should only be called inside a renderpass, within a frame.
     * 
     * @param Data The render data of the geometry to be drawn.
     */
    void (*DrawGeometry)(GeometryRenderData Data);

    /**
     * @brief Creates a Vulkan-specific texture, acquiring internal resources as needed.
     * 
     * @param Pixels The raw image data used for the texture.
     * @param Texture A pointer to the texture to hold the resources.
     */
    void (*CreateTexture)(const UInt8* Pixels, Texture* Texture);

    /**
     * @brief Destroys the given texture, releasing internal resources.
     * 
     * @param Texture A pointer to the texture to be destroyed.
     */
    void (*DestroyTexture)(struct Texture* Texture);

    /**
     * @brief Creates a material, acquiring required internal resources.
     * 
     * @param Material A pointer to the material to hold the resources.
     * @return True on success; otherwise false.
     */
    Bool8 (*CreateMaterial)(struct Material* Material);

    /**
     * @brief Destroys a texture, releasing required internal resouces.
     * 
     * @param Material A pointer to the material whose resources should be released.
     */
    void (*DestroyMaterial)(struct Material* Material);

    /**
     * @brief Creates Vulkan-specific internal resources for the given geometry using
     * the data provided.
     * 
     * @param Geometry A pointer to the geometry to be created.
     * @param VertexSize The size of a single vertex.
     * @param Vertex_Count The total number of vertices.
     * @param Vertices An array of vertices.
     * @param IndexSize The size of an individual index.
     * @param IndexCount The total number of indices.
     * @param Indices An array of indices.
     * @return True on success; otherwise false.
     */
    Bool8 (*CreateGeometry)(struct Geometry* Geometry, UInt32 VertexSize, UInt32 VertexCount, const void* Vertices, UInt32 IndexSize, UInt32 IndexCount, const void* Indices);

    /**
     * @brief Destroys the given geometry, releasing internal resources.
     * 
     * @param Geometry A pointer to the geometry to be destroyed.
     */
    void (*DestroyGeometry)(struct Geometry* Geometry);
} CrystalBackend;

typedef struct RenderPacket {
    Float32 DeltaTime;

    UInt32 GeometryCount;
    GeometryRenderData* Geometries;

    UInt32 UIGeometryCount;
    GeometryRenderData* UIGeometries;
} RenderPacket;