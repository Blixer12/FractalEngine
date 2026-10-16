/**
 * @file ResourceDef.h
 * @author Blixer12
 * @brief This file contains the types for common resources the engine uses.
 * 
 * @version 1.0
 * @date 2026-10-11
 * 
 * @copyright Fractal Engine is Copyright (c) Blixer12 2026
 * 
 */

#pragma once

#include "Math/MathDef.h"

/** @brief Pre-defined resource types. */
typedef enum ResourceType {
    /** @brief Text resource type. */
    RESOURCE_TYPE_TEXT,
    /** @brief Binary resource type. */
    RESOURCE_TYPE_BINARY,
    /** @brief Image resource type. */
    RESOURCE_TYPE_IMAGE,
    /** @brief Material resource type. */
    RESOURCE_TYPE_MATERIAL,
    /** @brief Static Mesh resource type. */
    RESOURCE_TYPE_STATIC_MESH,
     /** @brief Custom resource type. Used by loaders outside the core engine. (e.g plugin data) */
    RESOURCE_TYPE_CUSTOM
} ResourceType;

/**
 * @brief A generic structure for a resource. All resource loaders
 * load data into these.
 */
typedef struct Resource {
    /** @brief The identifier of the loader which handles this resource */
    UInt32 LoaderID;
    /** @brief The name of the resource. */
    const char* Name;
    /** @brief The full file path of the resource. */
    char* FullPath;
    /** @brief The size of the resource data in bytes. */
    UInt64 DataSize;
    /** @brief The resource data. */
    void* Data;
} Resource;

//-----------
// Textures
//-----------

/**
 * @brief The maximum length of a texture name.
 */
constexpr UInt32 TextureNameMaxLength = 256;

/**
 * @brief A structure to hold image resource data. 
 */
typedef struct ImageResourceData {
    /** @brief The number of channels. (Like RGBA) */
    UInt8 ChannelCount;
    /** @brief The width of the image. */
    UInt32 Width;
    /** @brief The height of the image. */
    UInt32 Height;
    /** @brief The pixel data of the image. */
    UInt8* Pixels;
} ImageResourceData;

/**
 * @brief Represents a texture.
 */
typedef struct Texture {
    /** @brief The unique texture identifier. */
    UInt32 ID;
    /** @brief The texture width. */
    UInt32 Width;
    /** @brief The texture height. */
    UInt32 Height;
    /** @brief The number of channels in the texture. */
    UInt8 ChannelCount;
    /** @brief Indicates if the texture has transparency. */
    Bool8 HasTransparency;
    /** @brief The texture generation. Incremented every time the data is reloaded. */
    UInt32 Generation;
    /** @brief The texture name. */
    char Name[TextureNameMaxLength];
    /** @brief The raw texture data (pixels). */
    void* InternalData;
} Texture;

/** @brief A collection of texture uses identified by the Enumeration */
typedef enum TextureUse {
    /** @brief An unknown use. This is default, but should never actually be used. */
    TEXTURE_USE_UNKNOWN = 0x00,
    /** @brief The texture is used as a diffuse map. */
    TEXTURE_USE_MAP_DIFFUSE = 0x01,
} TextureUse;

/**
 * @brief A structure which maps a texture, use and
 * other properties.
 */
typedef struct TextureMap {
    /** @brief A pointer to a texture. */
    Texture* Texture;
    /** @brief The use of the texture */
    TextureUse Use;
} TextureMap;

//-----------
// Materials
//-----------

/** @brief The maximum length of a material name. */
constexpr UInt32 MaterialNameMaxLength = 256;

/**
 * @brief A collection of material types.
 * @deprecated This should probably store a shader id instead, and be bound that way.
 */
typedef enum MaterialType {
    /** A material used in the world. */
    MATERIAL_TYPE_WORLD,
    /** A material used in the UI */
    MATERIAL_TYPE_UI
} MaterialType;

/**
 * @brief Material configuration typically loaded from
 * a file or created in code to load a material from.
 */
typedef struct MaterialConfig {
    /** @brief The name of the material. */
    char Name[MaterialNameMaxLength];
    /** @brief The material type. */
    MaterialType Type;
    /** @brief Indicates if the material should be automatically released when no references to it remain. */
    Bool8 AutoRelease;
    /** @brief The Base color of the material. */
    Vec4 BaseColor;
    /** @brief The Base color map name. */
    char BaseColorMapName[TextureNameMaxLength];
} MaterialConfig;

/**
 * @brief A material, which represents various properties
 * of a surface in the world such as texture, colour,
 * bumpiness, shininess and more.
 */
typedef struct Material {
    /** @brief The material id. */
    UInt32 ID;
    /** @brief The internal material id. Used by the renderer backend to map to internal resources. */
    UInt32 InternalID;
    /** @brief The material generation. Incremented every time the material is changed. */
    UInt32 Generation;
    /** @brief The material type. */
    MaterialType Type;
    /** @brief The material name. */
    char Name[MaterialNameMaxLength];
    /** @brief The Base color */
    Vec4 BaseColor;
    /** @brief The Base color texture map */
    TextureMap BaseColorMap;
} Material;

//-----------
// Geometry
//-----------

/** @brief The maximum length of a geometry name. */
constexpr UInt32 GeometryNameMaxLength = 256;

/**
 * @brief Represents actual geometry in the world
 * typically (but not always, depending on use) paired with a material
 */
typedef struct Geometry {
    /** @brief The geometry identifier. */
    UInt32 ID;
    /** @brief The internal geometry identifier, used by the renderer backend to map to internal resources. */
    UInt32 InternalID;
    /** @brief The geometry generation. Incremented every time the geometry changes. */
    UInt32 Generation;
    /** @brief The geometry name. */
    char Name[GeometryNameMaxLength];
    /** @brief A pointer to the material associated with this geometry.. */
    Material* Material;
} Geometry;