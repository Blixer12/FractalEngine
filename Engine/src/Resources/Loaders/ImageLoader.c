#include "ImageLoader.h"

#include "Core/Logger.h"
#include "Core/FString.h"
#include "Core/Memory.h"

#include "Resources/ResourceDef.h"

#include "Systems/ResourceSystem.h"

#define STB_IMAGE_IMPLEMENTATION
#include "Vendor/StbImage.h"

Bool8 ImageLoaderLoad(struct ResourceLoader* Self, const char* Name, Resource* Resource)
{
    if (!Self || !Name || !Resource)
    {
        return false;
    }

    char* FormatString = "%s/%s/%s%s";  // No subfolders still...
    const Int32 RequiredChannelCount = 4; // Standard RGBA, practically used everywhere!
    stbi_set_flip_vertically_on_load(true); // The memory of a image is fliped in memory by default (idk why, but it is a fun fact!)
    char FullFilePath[4096]; // Probably wont overflow (unless you SERIOUSLY use a BUNCH of subfolders!)

    // TODO: Try different extensions, because not EVERY IMAGE IS PNG!!!!
    StringFormat(FullFilePath, FormatString, ResourceSystemBasePath(), Self->TypePath, Name, ".png");

    Int32 Width;
    Int32 Height;
    Int32 ChannelCount;

    // Extend this!
    UInt8* Data = stbi_load(
        FullFilePath,
        &Width,
        &Height,
        &ChannelCount,
        RequiredChannelCount);

    const char* FailReason = stbi_failure_reason();
    if (FailReason)
    {
       FLERROR("Image resource loader failed to load file '%s': %s", FullFilePath, FailReason);
       stbi__err(0, 0);

       if (Data) stbi_image_free(Data);

       return false;
    }

    if (!Data)
    {
        FLERROR("Image resource loader failed to load filw '%s'", FullFilePath);
        return false;
    }

    // Allocator probably here
    Resource->FullPath = StringDuplicate(FullFilePath);

    ImageResourceData* ResourceData = FMAllocate(sizeof(ImageResourceData), MEMORY_TAG_TEXTURE);
    ResourceData->Pixels = Data;
    ResourceData->Width = Width;
    ResourceData->Height = Height;
    ResourceData->ChannelCount = RequiredChannelCount;

    Resource->Data = ResourceData;
    Resource->DataSize = sizeof(ImageResourceData);
    Resource->Name = Name;

    return true;
}

void ImageLoaderUnload(struct ResourceLoader* Self, Resource* Resource)
{
    if (!Self || !Resource)
    {
        FLWARN("ImageLoaderUnload called with a nullptr for self or resource");
        return;
    }

    UInt32 PathLength = StringLength(Resource->FullPath);
    if (PathLength)
    {
        FMFree(Resource->FullPath, sizeof(char) * PathLength + 1, MEMORY_TAG_STRING);
    }

    if (Resource->Data)
    {
        FMFree(Resource->Data, Resource->DataSize, MEMORY_TAG_TEXTURE);
        Resource->Data = 0;
        Resource->DataSize = 0;
        Resource->LoaderID = InvalidID;
    }
}

ResourceLoader ImageResourceLoaderCreate()
{
    ResourceLoader Loader;
    Loader.Type = RESOURCE_TYPE_IMAGE;
    Loader.CustomType = 0;
    Loader.Load = ImageLoaderLoad;
    Loader.Unload = ImageLoaderUnload;
    Loader.TypePath = "Textures";

    return Loader;
}