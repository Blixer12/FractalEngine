#pragma once

#include "Renderer/RendererBackend.h"

Bool8 VulkanRendererInitialize(RendererBackend* Backend, const char* AppName, struct PlatformState* Platform);
void VulkanRendererShutdown(RendererBackend* Backend);

void VulkanRendererOnResized(RendererBackend* Backend, UInt16 Width, UInt16 Height);

Bool8 VulkanrRendererBeginFrame(RendererBackend* Backend, Float32 DeltaTime);
Bool8 VulkanrRendererEndFrame(RendererBackend* Backend, Float32 DeltaTime);