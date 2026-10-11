#pragma once

#include "Renderer/CrystalDef.inl"

typedef struct TextureSystemConfig {
    UInt32 MaxTextureCount;
} TextureSystemConfig;

constexpr char DefaultTextureName[] = "Default";

Bool8 TextureSystemInitialize(UInt64* MemoryRequirement, void* State, TextureSystemConfig Config);
void TextureSystemShutdown(void* State);

Texture* TextureSystemAcquire(const char* Name, Bool8 AutoRelease);
void TextureSystemRelease(const char* Name);

Texture* TextureSystemGetDefault();