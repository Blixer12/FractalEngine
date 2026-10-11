#include "BinaryLoader.h"

#include "Core/Logger.h"
#include "Core/FString.h"
#include "Core/Memory.h"

#include "Resources/ResourceDef.h"

#include "Systems/ResourceSystem.h"

#include "LoaderUtils.h"

#include "Math/FMath.h"

#include "Platform/Filesystem.h"

Bool8 TextLoaderLoad(struct ResourceLoader* Self, const char* Name, Resource* Resource)
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
    if (!FilesystemOpen(FullFilePath, FILE_MODE_READ, false, &File))
    {
        FLERROR("TextLoaderLoad - unable to open text file for reading: '%s'", FullFilePath);
        return false;
    }

    Resource->FullPath = StringDuplicate(FullFilePath);

    UInt64 FileSize = 0;
    if (!FilesystemSize(&File, &FileSize))
    {
        FLERROR("Unable to read text file: %s", FullFilePath);
        FilesystemClose(&File);
        return false;
    }

    char* ResourceData = FMAllocate(sizeof(char) * FileSize, MEMORY_TAG_ARRAY);
    UInt64 ReadSize = 0;
    if (!FilesystemReadAllText(&File, ResourceData, &ReadSize))
    {
        FLERROR("Unable to read text file: %s", FullFilePath);
        FilesystemClose(&File);
        return false;
    }

    FilesystemClose(&File);

    Resource->Data = ResourceData;
    Resource->DataSize = ReadSize;
    Resource->Name = Name;

    return true;
}

void TextLoaderUnload(struct ResourceLoader* Self, Resource* Resource)
{
    if (!ResourceUnload(Self, Resource, MEMORY_TAG_ARRAY))
    {
        FLWARN("MaterialLoaderUnload called w/ nullptr for self or resource");
    }
}

ResourceLoader TextResourceLoaderCreate()
{
    ResourceLoader Loader;
    Loader.Type = RESOURCE_TYPE_TEXT;
    Loader.CustomType = 0;
    Loader.Load = TextLoaderLoad;
    Loader.Unload = TextLoaderUnload;
    Loader.TypePath = "";

    return Loader;
}