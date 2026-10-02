#include "FMath.h"
#include "Platform/Platform.h"

#include <math.h>
#include <stdlib.h>

static Bool8 RandomSeed = false;

Float32 Fsin(Float32 x) {
    return sinf(x);
}

Float32 Fcos(Float32 x) {
    return cosf(x);
}

Float32 Ftan(Float32 x) {
    return tanf(x);
}

Float32 Facos(Float32 x) {
    return acosf(x);
}

Float32 Fsqrt(Float32 x) {
    return sqrtf(x);
}

Float32 Fabs(Float32 x) {
    return fabsf(x);
}

Int32 FRandom() {
    if (!RandomSeed) {
        srand((UInt32)PlatformGetAbsoluteTime());
        RandomSeed = true;
    }
    return rand();
}

Int32 FRandomInRange(Int32 min, Int32 max) {
    if (!RandomSeed) {
        srand((UInt32)PlatformGetAbsoluteTime());
        RandomSeed = true;
    }
    return (rand() % (max - min + 1)) + min;
}

Float32 FloatFRandom() {
    return (float)FRandom() / (Float32)RAND_MAX;
}

Float32 FloatFRandomInRange(Float32 min, Float32 max) {
    return min + ((float)FRandom() / ((Float32)RAND_MAX / (max - min)));
}