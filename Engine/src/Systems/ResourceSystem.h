#pragma once

#include "Resources/ResourceDef.h"

typedef struct ResourceSystemConfig {
    UInt32 MaxLoaderCount;
    // Relative
    char* AssetBasePath;
} ResourceSystemConfig;

typedef struct ResourceLoader {
    UInt32 ID;
    ResourceType Type;
    const char* CustomType;
    const char* TypePath;
    Bool8 (*Load)(struct ResourceLoader* Self, const char* Name, Resource* Resource);
    void (*Unload)(struct ResourceLoader* Self, Resource* Resource);
} ResourceLoader;

Bool8 ResourceSystemInitialize(UInt64* MemoryRequirement, void* State, ResourceSystemConfig Config);
void ResourceSystemShutdown(void* State);

FAPI Bool8 ResourceSystemRegisterLoader(ResourceLoader Loader);

FAPI Bool8 ResourceSystemLoad(const char* Name, ResourceType Type, Resource* Resource);
FAPI Bool8 ResourceSystemLoadCustom(const char* Name, const char* CustomType, Resource* Resource);

FAPI void ResourceSystemUnload(Resource* Resource);

FAPI const char* ResourceSystemBasePath();