#pragma once

#include "Defines.h"

#if defined(_MSC_VER) && !defined(__clang__) && (__STDC_VERSION__ < 202311L)
    #define typeof __typeof__
#endif

#define VECTOR_DEFAULT_CAPACITY 1
#define VECTOR_RESIZE_FACTOR 2

#define VectorCreate(Type) \
    _VectorCreate(VECTOR_DEFAULT_CAPACITY, sizeof(Type))

#define VectorReserve(Type, Capacity) \
    _VectorCreate(Capacity, sizeof(Type))

#define VectorDestroy(Array) _VectorDestroy(Array);

#define VectorAppend(Array, Value)            \
    {                                       \
        typeof(Value) temp = Value;         \
        Array = _VectorAppend(Array, &temp);  \
    }

#define VectorRemove(Array, ValuePtr) \
    _VectorRemove(Array, ValuePtr)

#define VectorInsertAt(Array, Index, Value)            \
    {                                                   \
        typeof(Value) temp = Value;                     \
        Array = _VectorInsertAt(Array, Index, &temp);   \
    }

#define VectorRemoveAt(Array, Index, ValuePtr) \
    _VectorRemoveAt(Array, Index, ValuePtr)

#define VectorClear(Array) \
    _VectorFieldSet(Array, 1, 0)

#define VectorCapacity(Array) \
    _VectorFieldGet(Array, 0)

#define VectorSize(Array) \
    _VectorFieldGet(Array, 1)

#define VectorStride(Array) \
    _VectorFieldGet(Array, 2)

#define VectorSizeSet(Array, Value) \
    _VectorFieldSet(Array, 1, Value)

// --- Count Helpers ---
#define VectorBegin(Array) (Array)
#define VectorEnd(Array) ((typeof(Array))((char*)(Array) + (VectorSize(Array) * VectorStride(Array))))


// --- PRIVATE FUNCTIONS ---
// Please do not call these functions, they are used for the Macros ONLY and should never be used anywhere else.
extern void* _VectorCreate(UInt64 Length, UInt64 Stride);
extern void  _VectorDestroy(void* Array);
extern void* _VectorAppend(void* Array, const void* ValuePtr);
extern void  _VectorRemove(void* Array, void* Dest);
extern void* _VectorInsertAt(void* Array, UInt64 Index, void* ValuePtr);
extern void* _VectorRemoveAt(void* Array, UInt64 Index, void* Dest);
extern UInt64 _VectorFieldGet(void* Array, UInt64 Field);
extern void     _VectorFieldSet(void* Array, UInt64 Field, UInt64 Value);