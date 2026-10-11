#include "BinaryLoader.h"

#include "Core/Logger.h"
#include "Core/FString.h"
#include "Core/Memory.h"

#include "Resources/ResourceDef.h"

#include "Systems/ResourceSystem.h"

#include "Math/FMath.h"

#include "Platform/Filesystem.h"

#include "LoaderUtils.h"

Bool8 BinaryLoaderLoad(struct ResourceLoader* Self, const char* Name, Resource* Resource)
{
    if (!Self || !Name || !Resource)
    {
        return false;
    }

    // TODO: should be located anywhere (once again, no organization)
    char* FormatString = "%s/%s/%s%s";
    char FullFilePath[4096];

    StringFormat(FullFilePath, FormatString, ResourceSystemBasePath(), Self->TypePath, Name, "");

    FileHandle File;
    if (!FilesystemOpen(FullFilePath, FILE_MODE_READ, true, &File))
    {
        FLERROR("BinaryLoaderLoad - unable to open file for binary reading: '%s'", FullFilePath);
        return false;
    }

    Resource->FullPath = StringDuplicate(FullFilePath);

    UInt64 FileSize = 0;
    if (!FilesystemSize(&File, &FileSize))
    {
        FLERROR("Unable to Binary read file: %s", FullFilePath);
        FilesystemClose(&File);
        return false;
    }

    UInt8* ResourceData = FMAllocate(sizeof(UInt8) * FileSize, MEMORY_TAG_ARRAY);
    UInt64 ReadSize = 0;
    if (!FilesystemReadAllBytes(&File, ResourceData, &ReadSize))
    {
        FLERROR("Unable to Binary read file: %s", FullFilePath);
        FilesystemClose(&File);
        return false;
    }

    FilesystemClose(&File);

    Resource->Data = ResourceData;
    Resource->DataSize = ReadSize;
    Resource->Name = Name;

    return true;
}

void BinaryLoaderUnload(struct ResourceLoader* Self, Resource* Resource)
{
    if (!ResourceUnload(Self, Resource, MEMORY_TAG_ARRAY))
    {
        FLWARN("BinaryLoaderUnload called w/ nullptr for self or resource");
    }
}

ResourceLoader BinaryResourceLoaderCreate()
{
    ResourceLoader Loader;
    Loader.Type = RESOURCE_TYPE_BINARY;
    Loader.CustomType = 0;
    Loader.Load = BinaryLoaderLoad;
    Loader.Unload = BinaryLoaderUnload;
    Loader.TypePath = "";

    return Loader;
}