#pragma once

#include "Renderer/CrystalBackend.h"
#include "Resources/ResourceDef.h"

Bool8 VulkanRendererInitialize(CrystalBackend* Backend, const char* AppName);
void VulkanRendererShutdown(CrystalBackend* Backend);

void VulkanRendererOnResized(CrystalBackend* Backend, UInt16 Width, UInt16 Height);

Bool8 VulkanRendererBeginFrame(CrystalBackend* Backend, Float32 DeltaTime);
Bool8 VulkanRendererEndFrame(CrystalBackend* Backend, Float32 DeltaTime);

void VulkanRendererUpdateGlobalWorldState(Mat4 Projection, Mat4 View, Vec3 ViewPosition, Vec4 AmbientColor, Int32 Mode);
void VulkanRendererUpdateGlobalUIState(Mat4 Projection, Mat4 View, Int32 Mode);

Bool8 VulkanRendererBeginRenderpass(struct CrystalBackend* Backend, UInt8 RenderpassID);
Bool8 VulkanRendererEndRenderpass(struct CrystalBackend* Backend, UInt8 RenderpassID);

void VulkanRendererDrawGeometry(GeometryRenderData Data);

void VulkanRendererCreateTexture(Texture* Texture, const UInt8* Pixels);
void VulkanRendererDestroyTexture(Texture* Texture);

Bool8 VulkanRendererCreateMaterial(struct Material* Material);
void VulkanRendererDestroyMaterial(struct Material* Material);

Bool8 VulkanRendererCreateGeometry(struct Geometry* Geometry, UInt32 VertexCount, const Vertex3D* Vertices, UInt32 IndexCount, const UInt32* Indices);
void VulkanRendererDestroyGeometry(struct Geometry* Geometry);