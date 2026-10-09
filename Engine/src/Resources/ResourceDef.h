#pragma once

#include "Math/MathDef.h"

//-----------
// Textures
//-----------

constexpr UInt32 TextureNameMaxLength = 256;

typedef struct Texture {
    UInt32 ID;
    UInt32 Width;
    UInt32 Height;
    UInt8 ChannelCount;
    Bool8 HasTransparency;
    UInt32 Generation;
    char Name[TextureNameMaxLength];
    void* InternalData;
} Texture;

typedef enum TextureUse {
    TEXTURE_USE_UNKNOWN = 0x00,
    TEXTURE_USE_MAP_DIFFUSE = 0x01,
} TextureUse;

typedef struct TextureMap {
    Texture* Texture;
    TextureUse Use;
} TextureMap;

//-----------
// Materials
//-----------

constexpr UInt32 MaterialNameMaxLength = 256;

typedef struct Material {
    UInt32 ID;
    UInt32 Generation;
    UInt32 InternalID;
    char Name[MaterialNameMaxLength];
    Vec4 BaseColor;
    TextureMap BaseColorMap;
} Material;