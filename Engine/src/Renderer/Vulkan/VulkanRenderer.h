#pragma once

#include "Renderer/CrystalBackend.h"

Bool8 VulkanRendererInitialize(CrystalBackend* Backend, const char* AppName);
void VulkanRendererShutdown(CrystalBackend* Backend);

void VulkanRendererOnResized(CrystalBackend* Backend, UInt16 Width, UInt16 Height);

Bool8 VulkanRendererBeginFrame(CrystalBackend* Backend, Float32 DeltaTime);
void VullkanRendererUpdateGlobalState(Mat4 Projection, Mat4 View, Vec3 ViewPosition, Vec4 AmbientColor, Int32 Mode);
Bool8 VulkanRendererEndFrame(CrystalBackend* Backend, Float32 DeltaTime);

void VulkanRendererUpdateObject(Mat4 Model);
