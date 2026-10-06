#pragma once

#include "Math/MathDef.h"

typedef struct Texture {
    UInt32 ID;
    UInt32 Width;
    UInt32 Height;
    UInt8 ChannelCount;
    Bool8 HasTransparency;
    UInt64 Generation;
    void* InternalData;
} Texture;