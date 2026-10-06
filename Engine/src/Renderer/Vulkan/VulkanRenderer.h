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

void VulkanRendererCreateTexture(
    const char* Name,
    Bool8 AutoRelease,
    Int32 Width,
    Int32 Height,
    Int32 ChannelCount,
    const UInt8* Pixels,
    Bool8 HasTransparency,
    Texture* Texture);

void VulkanRendererDestroyTexture(Texture* Texture);