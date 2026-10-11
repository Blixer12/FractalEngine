#include "TextureSystem.h"

#include "Core/Logger.h"
#include "Core/FString.h"
#include "Core/Memory.h"
#include "Containers/Hashtable.h"

#include "Renderer/CrystalFrontend.h"

#include "Systems/ResourceSystem.h"

typedef struct TextureSystemState {
    TextureSystemConfig Config;
    Texture DefaultTexture;

    // Array of registered textures.
    Texture* RegisteredTextures;

    // Hashtable for texture lookups.
    Hashtable RegisteredTextureTable;
} TextureSystemState;

typedef struct TextureReference {
    UInt64 ReferenceCount;
    UInt32 Handle;
    Bool8 AutoRelease;
} TextureReference;

static TextureSystemState* StatePtr = 0;

Bool8 CreateDefaultTextures(TextureSystemState* State);
void DestroyDefaultTextures(TextureSystemState* State);
Bool8 LoadTexture(const char* TextureName, Texture* T);
void DestroyTexture(Texture* T);

Bool8 TextureSystemInitialize(UInt64* MemoryRequirement, void* State, TextureSystemConfig Config)
{
    if (Config.MaxTextureCount == 0) {
        FLFATAL("TextureSystemInitialize - Config.MaxTextureCount must be > 0.");
        return false;
    }

    // Block of memory will contain state structure, then block for array, then block for hashtable.
    UInt64 StructRequirement = sizeof(TextureSystemState);
    UInt64 ArrayRequirement = sizeof(Texture) * Config.MaxTextureCount;
    UInt64 HashtableRequirement = sizeof(TextureReference) * Config.MaxTextureCount;
    *MemoryRequirement = StructRequirement + ArrayRequirement + HashtableRequirement;

    if (!State) {
        return true;
    }

    StatePtr = State;
    StatePtr->Config = Config;

    // The array block is after the state. Already allocated, so just set the pointer.
    void* ArrayBlock = State + StructRequirement;
    StatePtr->RegisteredTextures = ArrayBlock;

    // Hashtable block is after array.
    void* HashtableBlock = ArrayBlock + ArrayRequirement;

    // Create a hashtable for texture lookups.
    HashtableCreate(sizeof(TextureReference), Config.MaxTextureCount, HashtableBlock, false, &StatePtr->RegisteredTextureTable);

    // Fill the hashtable with invalid references to use as a default.
    TextureReference InvalidReference;
    InvalidReference.AutoRelease = false;
    InvalidReference.Handle = InvalidID;  // Primary reason for needing default values.
    InvalidReference.ReferenceCount = 0;
    HashtableFill(&StatePtr->RegisteredTextureTable, &InvalidReference);

    // Invalidate all textures in the array.
    UInt32 Count = StatePtr->Config.MaxTextureCount;
    for (UInt32 i = 0; i < Count; ++i) {
        StatePtr->RegisteredTextures[i].ID = InvalidID;
        StatePtr->RegisteredTextures[i].Generation = InvalidID;
    }

    // Create default textures for use in the system.
    if (!CreateDefaultTextures(StatePtr))
    {
        FLFATAL("Failed to create default textures, Application cannot run!");
        return false;
    }

    return true;
}

void TextureSystemShutdown(void* State)
{
    (void)State;
    if (StatePtr) {
        // Destroy all loaded textures.
        for (UInt32 i = 0; i < StatePtr->Config.MaxTextureCount; ++i) {
            Texture* T = &StatePtr->RegisteredTextures[i];
            if (T->Generation != InvalidID) {
                CrystalDestroyTexture(T);
            }
        }

        DestroyDefaultTextures(StatePtr);

        StatePtr = 0;
    }
}

Texture* TextureSystemAcquire(const char* Name, Bool8 AutoRelease)
{
    if (StringsEqualI(Name, DefaultTextureName)) {
        FLWARN("TextureSystemAcquire called for default texture. Use TextureSystemGetDefault for texture 'Default'.");
        return &StatePtr->DefaultTexture;
    }

    TextureReference Reference;
    if (StatePtr && HashtableGet(&StatePtr->RegisteredTextureTable, Name, &Reference)) {
        // This can only be changed the first time a texture is loaded.
        if (Reference.ReferenceCount == 0) {
            Reference.AutoRelease = AutoRelease;
        }
        Reference.ReferenceCount++;
        if (Reference.Handle == InvalidID) {
            // This means no texture exists here. Find a free index first.
            UInt32 Count = StatePtr->Config.MaxTextureCount;
            Texture* T = 0;
            for (UInt32 i = 0; i < Count; ++i) {
                if (StatePtr->RegisteredTextures[i].ID == InvalidID) {
                    // A free slot has been found. Use its index as the handle.
                    Reference.Handle = i;
                    T = &StatePtr->RegisteredTextures[i];
                    break;
                }
            }

            // Make sure an empty slot was actually found.
            if (!T || Reference.Handle == InvalidID) {
                FLFATAL("TextureSystemAcquire - Texture system cannot hold anymore textures. Adjust configuration to allow more.");
                return 0;
            }

            // Create new texture.
            if (!LoadTexture(Name, T)) {
                FLERROR("Failed to load texture '%s'.", Name);
                return 0;
            }

            if (T->Generation == InvalidID) {
                T->Generation = 0;
            } else {
                T->Generation++;
            }

            // Also use the handle as the texture id.
            T->ID = Reference.Handle;
            FLTRACE("Texture '%s' does not yet exist. Created, and ReferenceCount is now %i.", Name, Reference.ReferenceCount);
        } else {
            FLTRACE("Texture '%s' already exists, ReferenceCount increased to %i.", Name, Reference.ReferenceCount);
        }

        // Update the entry.
        HashtableSet(&StatePtr->RegisteredTextureTable, Name, &Reference);
        return &StatePtr->RegisteredTextures[Reference.Handle];
    }

    // NOTE: This would only happen in the event something went wrong with the state.
    FLERROR("TextureSystemAcquire failed to acquire texture '%s'. Null pointer will be returned.", Name);
    return 0;
}

void TextureSystemRelease(const char* Name)
{
    // Ignore release requests for the default texture.
    if (StringsEqualI(Name, DefaultTextureName)) {
        return;
    }
    TextureReference Reference;
    if (StatePtr && HashtableGet(&StatePtr->RegisteredTextureTable, Name, &Reference)) {
        if (Reference.ReferenceCount == 0) {
            FLWARN("Tried to release non-existent texture: '%s'", Name);
            return;
        }

        char NameCopy[TextureNameMaxLength];
        StringNcopy(NameCopy, Name, TextureNameMaxLength);

        Reference.ReferenceCount--;
        if (Reference.ReferenceCount == 0 && Reference.AutoRelease) {
            Texture* T = &StatePtr->RegisteredTextures[Reference.Handle];

            // Destroy/Reset texture
            DestroyTexture(T);

            // Reset the reference.
            Reference.Handle = InvalidID;
            Reference.AutoRelease = false;
            FLTRACE("Released texture '%s'., Texture unloaded because reference count=0 and AutoRelease=true.", NameCopy);
        } else {
            FLTRACE("Released texture '%s', now has a reference count of '%i' (AutoRelease=%s).", NameCopy, Reference.ReferenceCount, Reference.AutoRelease ? "true" : "false");
        }

        // Update the entry.
        HashtableSet(&StatePtr->RegisteredTextureTable, NameCopy, &Reference);
    } else {
        FLERROR("TextureSystemRelease failed to release texture '%s'.", Name);
    }
}

Texture* TextureSystemGetDefault() 
{
    if (StatePtr) {
        return &StatePtr->DefaultTexture;
    }

    FLERROR("TextureSystemGetDefault called before texture system initialization! Null pointer returned.");
    return 0;
}

Bool8 CreateDefaultTextures(TextureSystemState* State)
{
    (void)State;
    // NOTE: Creates a default texture, 256x256 checkerboard on the fly!
    // Eliminates Asset Dependency! (Stored IN-MEMORY!! not on disc!!)

    FLDEBUG("Creating Default Texture");
    constexpr UInt32 TextureDimensions = 256;
    constexpr UInt32 Channels = 4;
    constexpr UInt32 PixelCount = TextureDimensions * TextureDimensions;
    UInt8 Pixels[PixelCount * Channels];

    FMSetMemory(Pixels, 255, sizeof(UInt8) * PixelCount * Channels);


    for (UInt64 Row = 0; Row < TextureDimensions; Row++)
    {
        for (UInt64 Column = 0; Column < TextureDimensions; Column++)
        {
            UInt64 Index = (Row * TextureDimensions) + Column;
            UInt64 IndexChannels = Index * Channels;
            if (Row % 2) {
                if (Column % 2) {
                    Pixels[IndexChannels + 0] = 128;
                    Pixels[IndexChannels + 1] = 0;
                    Pixels[IndexChannels + 2] = 255;
                }
            } else {
                if (!(Column % 2)) {
                    Pixels[IndexChannels + 0] = 128;
                    Pixels[IndexChannels + 1] = 0;
                    Pixels[IndexChannels + 2] = 255;
                }
            }
        } 
    }
    
    StringNcopy(State->DefaultTexture.Name, DefaultTextureName, TextureNameMaxLength);
    State->DefaultTexture.Width = TextureDimensions;
    State->DefaultTexture.Height = TextureDimensions;
    State->DefaultTexture.ChannelCount = Channels;
    State->DefaultTexture.Generation = InvalidID;
    State->DefaultTexture.HasTransparency = false;

    CrystalCreateTexture(Pixels, &StatePtr->DefaultTexture);

    StatePtr->DefaultTexture.Generation = InvalidID;

    return true;
}

void DestroyDefaultTextures(TextureSystemState* State) {
    if (State) {
        DestroyTexture(&State->DefaultTexture);
    }
}

void CreateTexture(Texture* T)
{
    FMZeroMemory(T, sizeof(Texture));
    T->Generation = InvalidID;
}

Bool8 LoadTexture(const char* TextureName, Texture* T)
{
    Resource ImageResource;
    if (!ResourceSystemLoad(TextureName, RESOURCE_TYPE_IMAGE, &ImageResource))
    {
        FLERROR("Failed to load image resources for texture '%s'", TextureName);
    }

    ImageResourceData* ResourceData = ImageResource.Data;

    Texture TemporaryTexture;
    TemporaryTexture.Width = ResourceData->Width;
    TemporaryTexture.Height = ResourceData->Height;
    TemporaryTexture.ChannelCount = ResourceData->ChannelCount;

    UInt32 CurrentGeneration = T->Generation;
    T->Generation = InvalidID;

    UInt64 TotalSize = TemporaryTexture.Width * TemporaryTexture.Height * TemporaryTexture.ChannelCount;
    Bool8 HasTransparency = false;

    for (UInt64 i = 0; i < TotalSize; i += TemporaryTexture.ChannelCount)
    {
        UInt8 a = ResourceData->Pixels[i + 3];
        if (a < 255)
        {
            HasTransparency = true;
            break;
        }
    }

    StringNcopy(TemporaryTexture.Name, TextureName, TextureNameMaxLength);
    TemporaryTexture.Generation = InvalidID;
    TemporaryTexture.HasTransparency = HasTransparency;

    CrystalCreateTexture(ResourceData->Pixels, &TemporaryTexture);

    Texture Old = *T;

    *T = TemporaryTexture;

    CrystalDestroyTexture(&Old);

    if (CurrentGeneration == InvalidID) {
        T->Generation = 0;
    } else {
        T->Generation = CurrentGeneration + 1;
    }

    ResourceSystemUnload(&ImageResource);
    return true;
}

void DestroyTexture(Texture* T)
{
    CrystalDestroyTexture(T);

    FMZeroMemory(T->Name, sizeof(char) * TextureNameMaxLength);
    FMZeroMemory(T, sizeof(Texture));
    T->ID = InvalidID;
    T->Generation = InvalidID;
}