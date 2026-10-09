#pragma once

#include "Renderer/CrystalBackend.h"
#include "Resources/ResourceDef.h"

Bool8 VulkanRendererInitialize(CrystalBackend* Backend, const char* AppName);
void VulkanRendererShutdown(CrystalBackend* Backend);

void VulkanRendererOnResized(CrystalBackend* Backend, UInt16 Width, UInt16 Height);

Bool8 VulkanRendererBeginFrame(CrystalBackend* Backend, Float32 DeltaTime);
void VullkanRendererUpdateGlobalState(Mat4 Projection, Mat4 View, Vec3 ViewPosition, Vec4 AmbientColor, Int32 Mode);
Bool8 VulkanRendererEndFrame(CrystalBackend* Backend, Float32 DeltaTime);

void VulkanRendererUpdateObject(GeometryRenderData* Data);

void VulkanRendererCreateTexture(const UInt8* Pixels, Texture* Texture);
void VulkanRendererDestroyTexture(Texture* Texture);

Bool8 VulkanRendererCreateMaterial(struct Material* Material);
void VulkanRendererDestroyMaterial(struct Material* Material);