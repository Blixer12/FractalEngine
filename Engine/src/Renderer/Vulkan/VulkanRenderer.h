#pragma once

#include "Renderer/CrystalBackend.h"

Bool8 VulkanRendererInitialize(CrystalBackend* Backend, const char* AppName, struct PlatformState* PlatformState);
void VulkanRendererShutdown(CrystalBackend* Backend);

void VulkanRendererOnResized(CrystalBackend* Backend, UInt16 Width, UInt16 Height);

Bool8 VulkanRendererBeginFrame(CrystalBackend* Backend, Float32 DeltaTime);
Bool8 VulkanRendererEndFrame(CrystalBackend* Backend, Float32 DeltaTime);
