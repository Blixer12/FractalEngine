#pragma once

#include "Defines.h"

#include "Resources/ResourceDef.h"

typedef struct MaterialSystemConfig {
    UInt32 MaxMaterialCount;
} MaterialSystemConfig;

constexpr char DefaultMaterialName[] = "Default";

Bool8 MaterialSystemInitialize(UInt64* MemoryRequirement, void* State, MaterialSystemConfig Config);
void MaterialSystemShutdown(void* State);

Material* MaterialSystemAcquire(const char* Name);
Material* MaterialSystemAcquireFromConfig(MaterialConfig Config);
void MaterialSystemRelease(const char* Name);

Material* MaterialSystemGetDefault();