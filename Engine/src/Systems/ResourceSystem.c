#include "ResourceSystem.h"

#include "Core/Logger.h"
#include "Core/FString.h"

// Resource loaders
#include "Resources/Loaders/TextLoader.h"
#include "Resources/Loaders/BinaryLoader.h"
#include "Resources/Loaders/ImageLoader.h"
#include "Resources/Loaders/MaterialLoader.h"

typedef struct ResourceSystemState {
    ResourceSystemConfig Config;
    ResourceLoader* RegisteredLoaders;
} ResourceSystemState;

static ResourceSystemState* StatePtr = 0;

Bool8 Load(const char* Name, ResourceLoader* Loader, Resource* Resource);

Bool8 ResourceSystemInitialize(UInt64* MemoryRequirement, void* State, ResourceSystemConfig Config)
{
    if (Config.MaxLoaderCount == 0)
    {
        FLFATAL("ResourceSystemInitialize failed because Config.MaxLoaderCount = 0");
    }

    *MemoryRequirement = sizeof(ResourceSystemState) + (sizeof(ResourceLoader) * Config.MaxLoaderCount);

    if (!State)
    {
        return true;
    }

    StatePtr = State;
    StatePtr->Config = Config;

    void* ArrayBlock = State + sizeof(ResourceSystemState);
    StatePtr->RegisteredLoaders = ArrayBlock;

    UInt32 Count = Config.MaxLoaderCount;
    for (UInt32 i = 0; i < Count; ++i)
    {
        StatePtr->RegisteredLoaders[i].ID = InvalidID;
    }

    ResourceSystemRegisterLoader(TextResourceLoaderCreate());
    ResourceSystemRegisterLoader(BinaryResourceLoaderCreate());
    ResourceSystemRegisterLoader(ImageResourceLoaderCreate());
    ResourceSystemRegisterLoader(MaterialResourceLoaderCreate());
    
    FLINFO("Resource system initialized with base path '%s'", Config.AssetBasePath);

    return true;
}

void ResourceSystemShutdown(void* State)
{
    (void)State;
    if (StatePtr)
    {
        StatePtr = 0;
    }
}

Bool8 ResourceSystemRegisterLoader(ResourceLoader Loader)
{
    if (StatePtr)
    {
        UInt32 Count = StatePtr->Config.MaxLoaderCount;

        for (UInt32 i = 0; i < Count; ++i)
        {
            ResourceLoader* L = &StatePtr->RegisteredLoaders[i];
            if (L->ID != InvalidID)
            {
                if (L->Type == Loader.Type) {
                    FLERROR("ResourceSystemRegisterLoad - Loader of type %d already exists and will not be registered", Loader.Type);
                    return false;
                } else if (Loader.CustomType && StringLength(Loader.CustomType) > 0 && StringsEqualI(L->CustomType, Loader.CustomType)) {
                    FLERROR("ResourceSystemRegisterLoad - Loader of custom type %s already exists and will not be registered", Loader.CustomType);
                    return false;
                }
            }
        }

        for (UInt32 i = 0; i < Count; ++i)
        {
            if (StatePtr->RegisteredLoaders[i].ID == InvalidID)
            {
                StatePtr->RegisteredLoaders[i] = Loader;
                StatePtr->RegisteredLoaders[i].ID = i;
                FLTRACE("Loader registered");
                return true;
            }
        }
    }
    return false;
}

Bool8 ResourceSystemLoad(const char* Name, ResourceType Type, Resource* Resource)
{
    if (StatePtr && Type != RESOURCE_TYPE_CUSTOM)
    {
        UInt32 Count = StatePtr->Config.MaxLoaderCount;
        for (UInt32 i = 0; i < Count; ++i)
        {
            ResourceLoader* L = &StatePtr->RegisteredLoaders[i];
            if (L->ID != InvalidID && L->Type == Type)
            {
                return Load(Name, L, Resource);
            }
        }
    }

    Resource->LoaderID = InvalidID;
    FLERROR("ResourceSystemLoad - No Loader for type %d was found", Type);
    return false;
}

Bool8 ResourceSystemLoadCustom(const char* Name, const char* CustomType, Resource* Resource)
{
    if (StatePtr && CustomType && StringLength(CustomType) > 0)
    {
        UInt32 Count = StatePtr->Config.MaxLoaderCount;
        for (UInt32 i = 0; i < Count; ++i)
        {
            ResourceLoader* L = &StatePtr->RegisteredLoaders[i];
            if (L->ID != InvalidID && L->Type == RESOURCE_TYPE_CUSTOM && StringsEqualI(L->CustomType, CustomType))
            {
                return Load(Name, L, Resource);
            }
        }
    }

    Resource->LoaderID = InvalidID;
    FLERROR("ResourceSystemLoadCustom - No Loader for type %s was found", CustomType);
    return false;
}

void ResourceSystemUnload(Resource* Resource)
{
    if (StatePtr && Resource)
    {
        if (Resource->LoaderID != InvalidID)
        {
            ResourceLoader* L = &StatePtr->RegisteredLoaders[Resource->LoaderID];
            if (L->ID != InvalidID && L->Unload)
            {
                L->Unload(L, Resource);
            }
        }
    }
}

const char* ResourceSystemBasePath()
{
    if (StatePtr)
    {
        return StatePtr->Config.AssetBasePath;
    }

    FLERROR("ResourceSystemBasePath called before init, returning empty string");
    return "";
}

Bool8 Load(const char* Name, ResourceLoader* Loader, Resource* Resource)
{
    if (!Name || !Loader || !Loader->Load || !Resource)
    {
        Resource->LoaderID = InvalidID;
        return false;
    }

    Resource->LoaderID = Loader->ID;
    return Loader->Load(Loader, Name, Resource);
}