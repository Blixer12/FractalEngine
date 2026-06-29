#include "_Vector.h"
#include "Vector.h"

#include "Core/Memory.h"
#include "Core/Logger.h"

void* _VectorCreate(UInt64 Length, UInt64 Stride)
{
    UInt64 HeaderSize = FIELD_LENGTH * sizeof(UInt64);
    UInt64 ArraySize = Length * Stride;
    UInt64* NewArray = FMAllocate(HeaderSize + ArraySize, MEMORY_TAG_VECTOR);
    FMSetMemory(NewArray, 0, HeaderSize + ArraySize);
    NewArray[VECTOR_CAPACITY] = Length;
    NewArray[VECTOR_SIZE] = 0;
    NewArray[VECTOR_STRIDE] = Stride;
    return (void*)(NewArray + FIELD_LENGTH);
}

void _VectorDestroy(void* Array)
{
    UInt64* Header = (UInt64*)Array - FIELD_LENGTH;
    UInt64 HeaderSize = FIELD_LENGTH * sizeof(UInt64);
    UInt64 TotalSize = HeaderSize + Header[VECTOR_CAPACITY] * Header[VECTOR_STRIDE];
    FMFree(Header, TotalSize, MEMORY_TAG_VECTOR);
}


UInt64 _VectorFieldGet(void* Array, UInt64 Field)
{
    UInt64* Header = (UInt64*)Array - FIELD_LENGTH;
    return Header[Field];
}

void _VectorFieldSet(void* Array, UInt64 Field, UInt64 Value)
{
    UInt64* Header = (UInt64*)Array - FIELD_LENGTH;
    Header[Field] = Value;
}


void* _VectorResize(void* Array)
{
    UInt64 Size = VectorSize(Array);
    UInt64 Stride = VectorStride(Array);
    void* Temp = _VectorCreate(
        (VECTOR_RESIZE_FACTOR * VectorCapacity(Array)),
        Stride);
    FMCopyMemory(Temp, Array, Size * Stride);

    _VectorFieldSet(Temp, VECTOR_SIZE, Size);
    _VectorDestroy(Array);
    return Temp;
}


void* _VectorAppend(void* Array, const void* ValuePtr)
{
    UInt64 Size = VectorSize(Array);
    UInt64 Stride = VectorStride(Array);
    if (Size >= VectorCapacity(Array)) {
        Array = _VectorResize(Array);
    }

    UInt64 Address = (UInt64)Array;
    Address += (Size * Stride);
    FMCopyMemory((void*)Address, ValuePtr, Stride);
    _VectorFieldSet(Array, VECTOR_SIZE, Size + 1);
    return Array;
}

void _VectorRemove(void* Array, void* Dest)
{
    UInt64 Size = VectorSize(Array);
    UInt64 Stride = VectorStride(Array);
    UInt64 Address = (UInt64)Array;
    Address += ((Size - 1) * Stride);
    FMCopyMemory(Dest, (void*)Address, Stride);
    _VectorFieldSet(Array, VECTOR_SIZE, Size - 1);
}


void* _VectorRemoveAt(void* Array, UInt64 Index, void* Dest)
{
    UInt64 Size = VectorSize(Array);
    UInt64 Stride = VectorStride(Array);
    if (Index >= Size) {
        FLERROR("Index outside the bounds of this array! Size: %llu, Index: %llu", Size, Index);
        return Array;
    }

    UInt64 Address = (UInt64)Array;
    FMCopyMemory(Dest, (void*)(Address + (Index * Stride)), Stride);

    // If not on the last element, snip out the entry and copy the rest inward.
    if (Index != Size - 1) {
        FMCopyMemory(
            (void*)(Address + (Index * Stride)),
            (void*)(Address + ((Index + 1) * Stride)),
            Stride * (Size - Index - 1));
    }

    _VectorFieldSet(Array, VECTOR_SIZE, Size - 1);
    return Array;
}

void* _VectorInsertAt(void* Array, UInt64 Index, void* ValuePtr)
{
    UInt64 Size = VectorSize(Array);
    UInt64 Stride = VectorStride(Array);
    if (Index > Size) {
        FLERROR("Index outside the bounds of this array! Length: %llu, index: %llu", Size, Index);
        return Array;
    }
    if (Size >= VectorCapacity(Array)) {
        Array = _VectorResize(Array);
    }

    UInt64 Address = (UInt64)Array;

    // If not on the last element, copy the rest outward.
    if (Index != Size - 1) {
        FMCopyMemory(
            (void*)(Address + ((Index + 1) * Stride)),
            (void*)(Address + (Index * Stride)),
            Stride * (Size - Index));
    }

    // Set the value at the index
    FMCopyMemory((void*)(Address + (Index * Stride)), ValuePtr, Stride);

    _VectorFieldSet(Array, VECTOR_SIZE, Size + 1);
    return Array;
}