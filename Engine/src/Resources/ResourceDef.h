#pragma once

#include "Math/MathDef.h"

// Predefined resource types
typedef enum ResourceType {
    RESOURCE_TYPE_TEXT,
    RESOURCE_TYPE_BINARY,
    RESOURCE_TYPE_IMAGE,
    RESOURCE_TYPE_MATERIAL,
    RESOURCE_TYPE_STATIC_MESH,
    RESOURCE_TYPE_CUSTOM
} ResourceType;

typedef struct Resource {
    UInt32 LoaderID;
    const char* Name;
    char* FullPath;
    UInt64 DataSize;
    void* Data;
} Resource;

//-----------
// Textures
//-----------

constexpr UInt32 TextureNameMaxLength = 256;

typedef struct ImageResourceData {
    UInt8 ChannelCount;
    UInt32 Width;
    UInt32 Height;
    UInt8* Pixels;
} ImageResourceData;

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

typedef struct MaterialConfig {
    char Name[MaterialNameMaxLength];
    Bool8 AutoRelease;
    Vec4 BaseColor;
    char BaseColorMapName[TextureNameMaxLength];
} MaterialConfig;

typedef struct Material {
    UInt32 ID;
    UInt32 InternalID;
    UInt32 Generation;
    char Name[MaterialNameMaxLength];
    Vec4 BaseColor;
    TextureMap BaseColorMap;
} Material;

//-----------
// Geometry
//-----------

constexpr UInt32 GeometryNameMaxLength = 256;

/**
 * @brief Represents actual geometry in the world
 * typically (but not always, depending on use) paired with a material
 */
typedef struct Geometry {
    UInt32 ID;
    UInt32 InternalID;
    UInt32 Generation;
    char Name[GeometryNameMaxLength];
    Material* Material;
} Geometry;