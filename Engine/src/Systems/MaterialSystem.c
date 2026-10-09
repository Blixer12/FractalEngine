#include "MaterialSystem.h"

#include "Core/Logger.h"
#include "Core/FString.h"

#include "Containers/Hashtable.h"

#include "Math/FMath.h"

#include "Renderer/CrystalFrontend.h"

#include "Systems/TextureSystem.h"

// TODO: Resource system
#include "Platform/Filesystem.h"
// TODO: End temporary

typedef struct MaterialSystemState {
    MaterialSystemConfig Config;
    Material DefaultMaterial;

    // Array of registered materials.
    Material* RegisteredMaterials;

    // Hashtable for material lookups.
    Hashtable RegisteredMaterialTable;
} MaterialSystemState;

typedef struct MaterialReference {
    UInt64 ReferenceCount;
    UInt32 Handle;
    Bool8 AutoRelease;
} MaterialReference;

static MaterialSystemState* StatePtr = 0;

Bool8 CreateDefaultMaterial(MaterialSystemState* State);
Bool8 LoadMaterial(MaterialConfig Config, Material* M);
void DestroyMaterial(Material* M);
Bool8 LoadConfigurationFile(const char* Path, MaterialConfig* Config);

Bool8 MaterialSystemInitialize(UInt64* MemoryRequirement, void* State, MaterialSystemConfig Config)
{
    if (Config.MaxMaterialCount == 0) {
        FLFATAL("MaterialSystemInitialize - Config.MaxMaterialCount must be > 0.");
        return false;
    }

    // Block of memory will contain state structure, then block for array, then block for hashtable.
    UInt64 StructRequirement = sizeof(MaterialSystemState);
    UInt64 ArrayRequirement = sizeof(Material) * Config.MaxMaterialCount;
    UInt64 HashtableRequirement = sizeof(MaterialReference) * Config.MaxMaterialCount;
    *MemoryRequirement = StructRequirement + ArrayRequirement + HashtableRequirement;

    if (!State) {
        return true;
    }

    StatePtr = State;
    StatePtr->Config = Config;

    // The array block is after the state. Already allocated, so just set the pointer.
    void* ArrayBlock = State + StructRequirement;
    StatePtr->RegisteredMaterials = ArrayBlock;

    // Hashtable block is after array.
    void* HashtableBlock = ArrayBlock + ArrayRequirement;

    // Create a hashtable for material lookups.
    HashtableCreate(sizeof(MaterialReference), Config.MaxMaterialCount, HashtableBlock, false, &StatePtr->RegisteredMaterialTable);

    // Fill the hashtable with invalid references to use as a default.
    MaterialReference InvalidReference;
    InvalidReference.AutoRelease = false;
    InvalidReference.Handle = InvalidID;  // Primary reason for needing default values.
    InvalidReference.ReferenceCount = 0;
    HashtableFill(&StatePtr->RegisteredMaterialTable, &InvalidReference);

    // Invalidate all materials in the array.
    UInt32 Count = StatePtr->Config.MaxMaterialCount;
    for (UInt32 i = 0; i < Count; ++i) {
        StatePtr->RegisteredMaterials[i].ID = InvalidID;
        StatePtr->RegisteredMaterials[i].InternalID = InvalidID;
        StatePtr->RegisteredMaterials[i].Generation = InvalidID;
    }

    // Create default material for use in the system.
    if (!CreateDefaultMaterial(StatePtr))
    {
        FLFATAL("Failed to create default material, Application cannot run!");
        return false;
    }

    return true;
}

void MaterialSystemShutdown(void* State)
{
    MaterialSystemState* S = (MaterialSystemState*)State;
    if (StatePtr) 
    {
        DestroyMaterial(&S->DefaultMaterial);
        StatePtr = 0;
    }
}

Material* MaterialSystemAcquire(const char* Name)
{
    // Load given material config from disk
    MaterialConfig Config = {0};

    // Load the file from disk
    // TODO: should be located anywhere (once again, no organization)
    char* FormatString = "Assets/Materials/%s.%s";
    char FullFilePath[4096];

    // TODO: try different extensions, again...
    StringFormat(FullFilePath, FormatString, Name, "fmat");
    if (!LoadConfigurationFile(FullFilePath, &Config))
    {
        FLERROR("Failed to load material file: '%s' Null pointer will be returned", FullFilePath);
        return 0;
    }

    return MaterialSystemAcquireFromConfig(Config);
}

Material* MaterialSystemAcquireFromConfig(MaterialConfig Config)
{
    if (StringsEqualI(Config.Name, DefaultTextureName)) {
        return &StatePtr->DefaultMaterial;
    }

    MaterialReference Reference;
    if (StatePtr && HashtableGet(&StatePtr->RegisteredMaterialTable, Config.Name, &Reference)) {
        // This can only be changed the first time a texture is loaded.
        if (Reference.ReferenceCount == 0) {
            Reference.AutoRelease = Config.AutoRelease;
        }

        Reference.ReferenceCount++;
        if (Reference.Handle == InvalidID) {
            // This means no texture exists here. Find a free index first.
            UInt32 Count = StatePtr->Config.MaxMaterialCount;
            Material* M = 0;
            for (UInt32 i = 0; i < Count; ++i) {
                if (StatePtr->RegisteredMaterials[i].ID == InvalidID) {
                    // A free slot has been found. Use its index as the handle.
                    Reference.Handle = i;
                    M = &StatePtr->RegisteredMaterials[i];
                    break;
                }
            }

            // Make sure an empty slot was actually found.
            if (!M || Reference.Handle == InvalidID) {
                FLFATAL("MaterialSystemAcquire - Texture system cannot hold anymore textures. Adjust configuration to allow more.");
                return 0;
            }

            // Create new texture.
            if (!LoadMaterial(Config, M)) {
                FLERROR("Failed to load material '%s'.", Config.Name);
                return 0;
            }

            if (M->Generation == InvalidID) {
                M->Generation = 0;
            } else {
                M->Generation++;
            }

            // Also use the handle as the texture id.
            M->ID = Reference.Handle;
            FLTRACE("Material '%s' does not yet exist. Created, and ReferenceCount is now %i.", Config.Name, Reference.ReferenceCount);
        } else {
            FLTRACE("Material '%s' already exists, ReferenceCount increased to %i.", Config.Name, Reference.ReferenceCount);
        }

        // Update the entry.
        HashtableSet(&StatePtr->RegisteredMaterialTable, Config.Name, &Reference);
        return &StatePtr->RegisteredMaterials[Reference.Handle];
    }

    // NOTE: This would only happen in the event something went wrong with the state.
    FLERROR("MaterialSystemAcquire failed to acquire material '%s'. Null pointer will be returned.", Config.Name);
    return 0;
}

void MaterialSystemRelease(const char* Name)
{
    // Ignore release requests for the default texture.
    if (StringsEqualI(Name, DefaultMaterialName)) {
        return;
    }
    MaterialReference Reference;
    if (StatePtr && HashtableGet(&StatePtr->RegisteredMaterialTable, Name, &Reference)) {
        if (Reference.ReferenceCount == 0) {
            FLWARN("Tried to release non-existent material: '%s'", Name);
            return;
        }

        char NameCopy[TextureNameMaxLength];
        StringNcopy(NameCopy, Name, TextureNameMaxLength);

        Reference.ReferenceCount--;
        if (Reference.ReferenceCount == 0 && Reference.AutoRelease) {
            Material* M = &StatePtr->RegisteredMaterials[Reference.Handle];

            // Destroy/Reset texture
            DestroyMaterial(M);

            // Reset the reference.
            Reference.Handle = InvalidID;
            Reference.AutoRelease = false;
            FLTRACE("Released material '%s'., Material unloaded because reference count=0 and AutoRelease=true.", NameCopy);
        } else {
            FLTRACE("Released material '%s', now has a reference count of '%i' (AutoRelease=%s).", NameCopy, Reference.ReferenceCount, Reference.AutoRelease ? "true" : "false");
        }

        // Update the entry.
        HashtableSet(&StatePtr->RegisteredMaterialTable, NameCopy, &Reference);
    } else {
        FLERROR("MaterialSystemRelease failed to release material '%s'.", Name);
    }
}

Bool8 LoadMaterial(MaterialConfig Config, Material* M)
{
    FMZeroMemory(M, sizeof(Material));

    // Name
    StringNcopy(M->Name, Config.Name, MaterialNameMaxLength);

    // Base color
    M->BaseColor = Config.BaseColor;

    // Base Color Map
    if (StringLength(Config.BaseColorMapName) > 0)
    {
        M->BaseColorMap.Use = TEXTURE_USE_MAP_DIFFUSE;
        M->BaseColorMap.Texture = TextureSystemAcquire(Config.BaseColorMapName, true);
        if (!M->BaseColorMap.Texture)
        {
            FLWARN("Unable to load texture '%s' for material '%s', using default", Config.BaseColorMapName, M->Name);
            M->BaseColorMap.Texture = TextureSystemGetDefaultTexture();
        }
    } else {
        // NOTE: Sets for clarity
        M->BaseColorMap.Use = TEXTURE_USE_UNKNOWN;
        M->BaseColorMap.Texture = 0;
    }

    // TODO: Other maps like normals

    // Send it to renderer to acquire resources
    if (!CrystalCreateMaterial(M))
    {
        FLERROR("Failed to acquire crystal's resources for material '%s'", M->Name);
        return false;
    }

    return true;
}

void DestroyMaterial(Material* M)
{
    FLTRACE("Destroying material '%s'...", M->Name);

    if (M->BaseColorMap.Texture)
    {
        TextureSystemRelease(M->BaseColorMap.Texture->Name);
    }

    CrystalDestroyMaterial(M);

    FMZeroMemory(M, sizeof(Material));
    M->ID = InvalidID;
    M->InternalID = InvalidID;
    M->Generation = InvalidID;
}

Bool8 CreateDefaultMaterial(MaterialSystemState* State)
{
    FMZeroMemory(&State->DefaultMaterial, sizeof(Material));
    State->DefaultMaterial.ID = InvalidID;
    State->DefaultMaterial.Generation = InvalidID;
    StringNcopy(State->DefaultMaterial.Name, DefaultMaterialName, MaterialNameMaxLength);
    State->DefaultMaterial.BaseColor = Vec4One(); // White
    State->DefaultMaterial.BaseColorMap.Use = TEXTURE_USE_MAP_DIFFUSE;
    State->DefaultMaterial.BaseColorMap.Texture = TextureSystemGetDefaultTexture();

    if (!CrystalCreateMaterial(&State->DefaultMaterial))
    {
        FLFATAL("Failed to acquire crystal's resources for default material. Application cannot continue");
        return false;
    }

    return true;
}

Bool8 LoadConfigurationFile(const char* Path, MaterialConfig* Config)
{
    FileHandle File;
    if (!FilesystemOpen(Path, FILE_MODE_READ, false, &File))
    {
        FLERROR("LoadConfigurationFile - Unable to open material for reading: '%s", Path);
        return false;
    }
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
            FLWARN("Potential formattinmg issue in file '%s': '=' token not found, skipping line %ui.", Path, LineNumber);
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
            StringNcopy(Config->Name, TrimmedValue, MaterialNameMaxLength);
        } else if (StringsEqualI(TrimmedVariableName, "BaseColorMapName")) {
            StringNcopy(Config->BaseColorMapName, TrimmedValue, TextureNameMaxLength);
        } else if (StringsEqualI(TrimmedVariableName, "BaseColor")) {
            // Parse the color
            if (!StringToVec4(TrimmedValue, &Config->BaseColor))
            {
                FLWARN("Error parsing BaseColor in file '%s'. Using default of white instead.", Path);
                Config->BaseColor = Vec4One();
            }
        } else if (StringsEqualI(TrimmedVariableName, "AutoRelease")) {
            Config->AutoRelease = StringToBool(TrimmedValue, &Config->AutoRelease);
        } 

        // More fields

        FMZeroMemory(LineBuffer, sizeof(char) * 1538);
        LineNumber++;
    }

    FilesystemClose(&File);

    return true;
}