/**
 * @file MathDef.h
 * @author Blixer12
 * @brief Contains various math types required for the engine.
 * 
 * @version 1.0
 * @date 2026-10-11
 * 
 * @copyright Fractal Engine is Copyright (c) Blixer12 2026
 */

#pragma once

#include "Defines.h"

/**
 * @brief A 2-element vector supporting spatial (x, y), color (r, g), and texture (s/u, t/v) access.
 */
typedef union Vec2U {
    /** @brief An array of x, y components. */
    Float32 Elements[2];

#ifdef DOXYGEN
    /** @brief The first element (X position). */
    Float32 x;
    /** @brief The first element (Red color). */
    Float32 r;
    /** @brief The first element (Texture S). */
    Float32 s;
    /** @brief The first element (Texture U). */
    Float32 u;
    /** @brief The second element (Y position). */
    Float32 y;
    /** @brief The second element (Green color). */
    Float32 g;
    /** @brief The second element (Texture T). */
    Float32 t;
    /** @brief The second element (Texture V). */
    Float32 v;
#else
    struct {
        union {
            Float32 x; /**< The first element (X position). */
            Float32 r; /**< The first element (Red color). */
            Float32 s; /**< The first element (Texture S). */
            Float32 u; /**< The first element (Texture U). */
        };
        union {
            Float32 y; /**< The second element (Y position). */
            Float32 g; /**< The second element (Green color). */
            Float32 t; /**< The second element (Texture T). */
            Float32 v; /**< The second element (Texture V). */
        };
    };
#endif
} Vec2;

/**
 * @brief A 3-element vector supporting spatial (x, y, z), color (r, g, b), and texture access.
 */
typedef union Vec3U {
    /** @brief An array of x, y, z components. */
    Float32 Elements[3];

#ifdef DOXYGEN
    /** @brief The first element (X position). */
    Float32 x;
    /** @brief The first element (Red color). */
    Float32 r;
    /** @brief The first element (Texture S). */
    Float32 s;
    /** @brief The first element (Texture U). */
    Float32 u;
    /** @brief The second element (Y position). */
    Float32 y;
    /** @brief The second element (Green color). */
    Float32 g;
    /** @brief The second element (Texture T). */
    Float32 t;
    /** @brief The second element (Texture V). */
    Float32 v;
    /** @brief The third element (Z position). */
    Float32 z;
    /** @brief The third element (Blue color). */
    Float32 b;
    /** @brief The third element (Texture P). */
    Float32 p;
    /** @brief The third element (Depth/Width). */
    Float32 w;
#else
    struct {
        union {
            Float32 x; /**< The first element (X position). */
            Float32 r; /**< The first element (Red color). */
            Float32 s; /**< The first element (Texture S). */
            Float32 u; /**< The first element (Texture U). */
        };
        union {
            Float32 y; /**< The second element (Y position). */
            Float32 g; /**< The second element (Green color). */
            Float32 t; /**< The second element (Texture T). */
            Float32 v; /**< The second element (Texture V). */
        };
        union {
            Float32 z; /**< The third element (Z position). */
            Float32 b; /**< The third element (Blue color). */
            Float32 p; /**< The third element (Texture P). */
            Float32 w; /**< The third element (Depth/Width). */
        };
    };
#endif
} Vec3;

/**
 * @brief A 4-element vector supporting spatial, RGBA color, and homogeneous coordinates.
 */
typedef union Vec4U {
    /** @brief An array of x, y, z, w components. */
    alignas(16) Float32 Elements[4];

#ifdef DOXYGEN
    /** @brief The first element (X position). */
    Float32 x;
    /** @brief The first element (Red color). */
    Float32 r;
    /** @brief The first element (Texture S). */
    Float32 s;
    /** @brief The second element (Y position). */
    Float32 y;
    /** @brief The second element (Green color). */
    Float32 g;
    /** @brief The second element (Texture T). */
    Float32 t;
    /** @brief The third element (Z position). */
    Float32 z;
    /** @brief The third element (Blue color). */
    Float32 b;
    /** @brief The third element (Texture P). */
    Float32 p;
    /** @brief The fourth element (W position). */
    Float32 w;
    /** @brief The fourth element (Alpha color). */
    Float32 a;
    /** @brief The fourth element (Texture Q). */
    Float32 q;
#else
    struct {
        union {
            Float32 x; /**< The first element (X position). */
            Float32 r; /**< The first element (Red color). */
            Float32 s; /**< The first element (Texture S). */
        };
        union {
            Float32 y; /**< The second element (Y position). */
            Float32 g; /**< The second element (Green color). */
            Float32 t; /**< The second element (Texture T). */
        };
        union {
            Float32 z; /**< The third element (Z position). */
            Float32 b; /**< The third element (Blue color). */
            Float32 p; /**< The third element (Texture P). */
        };
        union {
            Float32 w; /**< The fourth element (W position). */
            Float32 a; /**< The fourth element (Alpha color). */
            Float32 q; /**< The fourth element (Texture Q). */
        };
    };
#endif
} Vec4;

/** @brief A quaternion, used to represent 3D rotational orientation. */
typedef Vec4 Quaternion;

/** @brief A 4x4 matrix, typically used for object transformations and projections. */
typedef union Mat4U {
    /** @brief The matrix elements in column-major or row-major order. */
    Float32 Data[16];
} Mat4;

/**
 * @brief Represents a single vertex in 3D space.
 */
typedef struct Vertex3D {
    /** @brief The 3D position of the vertex. */
    Vec3 Position;
    /** @brief The 2D texture coordinates (UVs) of the vertex. */
    Vec2 TextureCoordinates;
} Vertex3D;

/**
 * @brief Represents a single vertex in 2D space.
 */
typedef struct Vertex2D {
    /** @brief The 2D position of the vertex. */
    Vec2 Position;
    /** @brief The 2D texture coordinates (UVs) of the vertex. */
    Vec2 TextureCoordinates;
} Vertex2D;