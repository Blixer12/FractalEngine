#pragma once

#include "Defines.h"

typedef union Vec2U {
    // An array of x, y
    Float32 Elements[2];
    struct {
        union {
            // The first element.
            Float32 x, r, s, u;
        };
        union {
            // The second element.
            Float32 y, g, t, v;
        };
    };
} Vec2;

typedef struct Vec3U {
    union {
        // An array of x, y, z
        Float32 Elements[3];
        struct {
            union {
                // The first element.
                Float32 x, r, s, u;
            };
            union {
                // The second element.
                Float32 y, g, t, v;
            };
            union {
                // The third element.
                Float32 z, b, p, w;
            };
        };
    };
} Vec3;

typedef union Vec4U {
    // An array of x, y, z, w
    alignas(16) Float32 Elements[4];
    union {
        struct {
            union {
                // The first element.
                Float32 x, r, s;
            };
            union {
                // The second element.
                Float32 y, g, t;
            };
            union {
                // The third element.
                Float32 z, b, p;
            };
            union {
                // The fourth element.
                Float32 w, a, q;
            };
        };
    };
} Vec4;

typedef Vec4 Quaternion;


typedef union Mat4U {
    alignas(16) Float32 Data[16];
} Mat4;

typedef struct Vertex3D {
    Vec3 Position;
} Vertex3D;