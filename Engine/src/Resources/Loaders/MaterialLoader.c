#include "MaterialLoader.h"

#include "Core/Logger.h"
#include "Core/FString.h"
#include "Core/Memory.h"

#include "Resources/ResourceDef.h"

#include "Systems/ResourceSystem.h"

#include "Math/FMath.h"

#include "Platform/Filesystem.h"

Bool8 MaterialLoaderLoad(struct ResourceLoader* Self, const char* Name, Resource* Resource)
{
    if (!Self || !Name || !Resource)
    {
        return false;
    }

    // TODO: should be located anywhere (once again, no organization)
    char* FormatString = "%s/%s/%s%s";
    char FullFilePath[4096];

    StringFormat(FullFilePath, FormatString, ResourceSystemBasePath(), Self->TypePath, Name, ".fmat");
    Resource->FullPath = StringDuplicate(FullFilePath);

    FileHandle File;
    if (!FilesystemOpen(FullFilePath, FILE_MODE_READ, false, &File))
    {
        FLERROR("MaterialLoaderLoad - unable to open material file for reading: '%s'", FullFilePath);
        return false;
    }

    MaterialConfig* ResourceData = FMAllocate(sizeof(MaterialConfig), MEMORY_TAG_MATERIAL_INSTANCE);
    ResourceData->AutoRelease = true;
    ResourceData->BaseColor = Vec4One();
    ResourceData->BaseColorMapName[0] = 0;
    StringNcopy(ResourceData->Name, Name, MaterialNameMaxLength);

    char LineBuffer[1538] = "";
    char* Pointer = &LineBuffer[0];
    UInt64 LineLength = 0;
    UInt32 LineNumber = 1;
    while (FilesystemReadLine(&File, 1537, &Pointer, &LineLength))
    {
        char* Trimmed = StringTrim(LineBuffer);

        LineLength = StringLength(Trimmed);

        if (LineLength < 1 || Trimmed[0] == '#')
        {
            LineNumber++;
            continue;
        }

        Int32 EqualIndex = StringIndexOf(Trimmed, '=');
        if (EqualIndex == -1)
        {
            FLWARN("Potential formattinmg issue in file '%s': '=' token not found, skipping line %ui.", FullFilePath, LineNumber);
            LineNumber++;
            continue;
        }

        char RawVariableName[512];
        FMZeroMemory(RawVariableName, sizeof(char) * 512);
        StringMid(RawVariableName, Trimmed, 0, EqualIndex);
        char* TrimmedVariableName = StringTrim(RawVariableName);

        char RawValue[1024];
        FMZeroMemory(RawValue, sizeof(char) * 1024);
        StringMid(RawValue, Trimmed, EqualIndex + 1, -1);
        char* TrimmedValue = StringTrim(RawValue);

        if (StringsEqualI(TrimmedVariableName, "Version")) {
            // TODO: Version
        } else if (StringsEqualI(TrimmedVariableName, "Name")) {
            StringNcopy(ResourceData->Name, TrimmedValue, MaterialNameMaxLength);
        } else if (StringsEqualI(TrimmedVariableName, "BaseColorMapName")) {
            StringNcopy(ResourceData->BaseColorMapName, TrimmedValue, TextureNameMaxLength);
        } else if (StringsEqualI(TrimmedVariableName, "BaseColor")) {
            // Parse the color
            if (!StringToVec4(TrimmedValue, &ResourceData->BaseColor))
            {
                FLWARN("Error parsing BaseColor in file '%s'. Using default of white instead.", FullFilePath);
            }
        } else if (StringsEqualI(TrimmedVariableName, "AutoRelease")) {
            ResourceData->AutoRelease = StringToBool(TrimmedValue, &ResourceData->AutoRelease);
        } 

        // More fields

        FMZeroMemory(LineBuffer, sizeof(char) * 1538);
        LineNumber++;
    }

    FilesystemClose(&File);

    Resource->Data = ResourceData;
    Resource->DataSize = sizeof(MaterialConfig);
    Resource->Name = Name;

    return true;

}

void MaterialLoaderUnload(struct ResourceLoader* Self, Resource* Resource)
{
    if (!Self || !Resource)
    {
        FLWARN("MaterialLoaderUnload called with a nullptr for self or resource");
        return;
    }

    UInt32 PathLength = StringLength(Resource->FullPath);
    if (PathLength)
    {
        FMFree(Resource->FullPath, sizeof(char) * PathLength + 1, MEMORY_TAG_STRING);
    }

    if (Resource->Data)
    {
        FMFree(Resource->Data, Resource->DataSize, MEMORY_TAG_MATERIAL_INSTANCE);
        Resource->Data = 0;
        Resource->DataSize = 0;
        Resource->LoaderID = InvalidID;
    }
}

ResourceLoader MaterialResourceLoaderCreate()
{
    ResourceLoader Loader;
    Loader.Type = RESOURCE_TYPE_MATERIAL;
    Loader.CustomType = 0;
    Loader.Load = MaterialLoaderLoad;
    Loader.Unload = MaterialLoaderUnload;
    Loader.TypePath = "Materials";

    return Loader;
}