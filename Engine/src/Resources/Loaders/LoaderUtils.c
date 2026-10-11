#include "LoaderUtils.h"

#include "Core/Logger.h"
#include "Core/FString.h"

Bool8 ResourceUnload(struct ResourceLoader* Self, Resource* Resource, MemoryTag Tag)
{

    if (!Self || !Resource)
    {
        FLWARN("ResourceUnload called with a nullptr for self or resource");
        return false;
    }

    UInt32 PathLength = StringLength(Resource->FullPath);
    if (PathLength)
    {
        FMFree(Resource->FullPath, sizeof(char) * PathLength + 1, MEMORY_TAG_STRING);
    }

    if (Resource->Data)
    {
        FMFree(Resource->Data, Resource->DataSize, Tag);
        Resource->Data = 0;
        Resource->DataSize = 0;
        Resource->LoaderID = InvalidID;
    }

    return true;

}