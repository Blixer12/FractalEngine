/**
 * @file CrystalFrontend.h
 * @author Blixer12
 * @brief The Crystal frontend, which is the only thing the rest of the engine sees.
 * This is responsible for transferring any data to and from the Crystal backend in an
 * agnostic way.
 * 
 * @version 1.0
 * @date 2026-10-11
 * 
 * @copyright Fractal Engine is Copyright (c) Blixer12 2026
 * 
 */

#pragma once

#include "CrystalDef.inl"

struct PlatformState;
struct StaticMeshData;

/**
 * @brief Initializes Crystal's frontend/system. Should be called twice - once
 * to obtain the memory requirement (passing state=0), and a second time passing
 * allocated memory to state.
 * 
 * @param MemoryRequirement A pointer to hold the memory requirement for this system.
 * @param State A block of memory to hold state data, or 0 if obtaining memory requirement.
 * @param AppName The name of the application.
 * @return True on success; otherwise false.
 */
Bool8 CrystalInitialize(UInt64* MemoryRequirement, void* State, const char* AppName);

/**
 * @brief Shuts Crystal's system/frontend down.
 */
void CrystalShutdown();

/**
 * @brief Handles resize events.
 * 
 * @param Width The new window width.
 * @param Height The new window height.
 */
void CrystalOnResize(UInt16 Width, UInt16 Height);

/**
 * @brief Draws the next frame using the data provided in the render packet.
 * 
 * @param Packet A pointer to the render packet, which contains data on what should be rendered.
 * @return True on success; otherwise false.
 */
Bool8 CrystalDrawFrame(RenderPacket* Packet);

/**
 * @brief Sets the view matrix in Crystal. NOTE: exposed to public API.
 * 
 * @deprecated HACK: this should not be exposed outside the engine.
 * @param View The view matrix to be set.
 */
FAPI void CrystalSetView(Mat4 View);

/**
 * @brief Creates a new texture.
 * 
 * @param Pixels The raw image data to be uploaded to the GPU.
 * @param Texture A pointer to the texture to be loaded.
 */
void CrystalCreateTexture(const UInt8* Pixels, struct Texture* Texture);

/**
 * @brief Destroys the given texture, releasing internal resources from the GPU.
 * 
 * @param Texture A pointer to the texture to be destroyed.
 */
void CrystalDestroyTexture(struct Texture* Texture);

/**
 * @brief Creates a new material instance, acquiring GPU resources.
 * 
 * @param Material A pointer to the material to load.
 * @return True on success; otherwise false.
 */
Bool8 CrystalCreateMaterial(struct Material* Material);

/**
 * @brief Destroys the given material, releasing GPU resources.
 * 
 * @param Material A pointer to the material to unload.
 */
void CrystalDestroyMaterial(struct Material* Material);

/**
 * @brief Acquiores GPU resources and uploads geometry data.
 * 
 * @param Geometry A pointer to the geometry to acquire resources for.
 * @param VertexSize The size of each vertex.
 * @param VertexCount The number of vertices.
 * @param Vertices The vertex array.
 * @param IndexSize The size of each index.
 * @param IndexCount The number of indices.
 * @param Indices The index array.
 * @return True on success; otherwise false.
 */
Bool8 CrystalCreateGeometry(struct Geometry* Geometry, UInt32 VertexSize, UInt32 VertexCount, const void* Vertices, UInt32 IndexSize, UInt32 IndexCount, const void* Indices);

/**
 * @brief Destroys the given geometry, releasing GPU resources.
 * 
 * @param Geometry A pointer to the geometry to be destroyed.
 */
void CrystalDestroyGeometry(struct Geometry* Geometry);