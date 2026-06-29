#pragma once

#include "Defines.h"

enum {
    VECTOR_CAPACITY,
    VECTOR_SIZE,
    VECTOR_STRIDE,
    FIELD_LENGTH
};

FAPI void* _VectorCreate(UInt64 Length, UInt64 Stride);
FAPI void _VectorDestroy(void* Array);

FAPI UInt64 _VectorFieldGet(void* Array, UInt64 Field);
FAPI void _VectorFieldSet(void* Array, UInt64 Field, UInt64 Value);

FAPI void* _VectorResize(void* Array);

FAPI void* _VectorAppend(void* Array, const void* ValuePtr);
FAPI void _VectorRemove(void* Array, void* Dest);

FAPI void* _VectorRemoveAt(void* Array, UInt64 Index, void* Dest);
FAPI void* _VectorInsertAt(void* Array, UInt64 Index, void* ValuePtr);