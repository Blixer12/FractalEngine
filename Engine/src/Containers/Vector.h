#pragma once

#include "Defines.h"

// --- COMPILER COMPATIBILITY ---
// C23 introduced the standard `typeof` keyword.
// For MSVC on older C standards (pre-C23), alias MSVC's GNU extension `__typeof__`
// so the `typeof(Value)` trick works across compilers.
#if defined(_MSC_VER) && !defined(__clang__) && (__STDC_VERSION__ < 202311L)
    #define typeof __typeof__
#endif

// --- CONFIGURATION CONSTANTS ---
#define VECTOR_DEFAULT_CAPACITY 1 // Default initial element capacity when created empty
#define VECTOR_RESIZE_FACTOR 2    // Multiplier for growth when reallocation occurs

// --- PUBLIC MACRO API ---

/**
 * @brief Creates a dynamic vector for a given type with default capacity (1).
 * @param Type The data type to store (e.g., int, MyStruct).
 * @return Pointer to the newly allocated typed array memory buffer.
 */
#define VectorCreate(Type) \
    _VectorCreate(VECTOR_DEFAULT_CAPACITY, sizeof(Type))

/**
 * @brief Creates a dynamic vector with a pre-allocated capacity.
 * @param Type The data type to store.
 * @param Capacity The initial number of elements to reserve space for.
 * @return Pointer to the allocated typed array memory buffer.
 */
#define VectorReserve(Type, Capacity) \
    _VectorCreate(Capacity, sizeof(Type))

/**
 * @brief Frees the vector and its hidden header memory.
 * @param Array The vector pointer to destroy.
 */
#define VectorDestroy(Array) _VectorDestroy(Array)

/**
 * @brief Appends an element to the end of the vector.
 * 
 * Uses a local temporary variable (`temp`) to create a stack-allocated addressable copy 
 * of `Value` so its pointer (`&temp`) can be passed into `_VectorAppend`.
 * 
 * @param Array The vector pointer (re-assigned in case memory was reallocated).
 * @param Value The item/lvalue/rvalue to push.
 */
#define VectorAppend(Array, Value)            \
    {                                         \
        typeof(Value) temp = Value;           \
        Array = _VectorAppend(Array, &temp);  \
    }

/**
 * @brief Removes an item from the vector matching a value/pointer criteria.
 * @param Array The vector pointer.
 * @param ValuePtr Pointer to the value to match/remove (or destination output).
 */
#define VectorRemove(Array, ValuePtr) \
    _VectorRemove(Array, ValuePtr)

/**
 * @brief Inserts an element at a specific zero-based index.
 * 
 * Creates a stack-allocated copy (`temp`) so temporary or literal values can be safely passed by reference.
 * 
 * @param Array The vector pointer (re-assigned in case memory was reallocated).
 * @param Index Zero-based index location to insert at.
 * @param Value The item to insert.
 */
#define VectorInsertAt(Array, Index, Value)            \
    {                                                   \
        typeof(Value) temp = Value;                     \
        Array = _VectorInsertAt(Array, Index, &temp);   \
    }

/**
 * @brief Removes an element at the specified index and optionally copies it out.
 * @param Array The vector pointer.
 * @param Index Zero-based index location to remove from.
 * @param ValuePtr Pointer to memory where the removed element will be copied (can be NULL if ignored).
 */
#define VectorRemoveAt(Array, Index, ValuePtr) \
    _VectorRemoveAt(Array, Index, ValuePtr)

/**
 * @brief Resets the element count to zero without freeing allocated memory capacity.
 * Sets Field index 1 (Length/Size) to 0.
 */
#define VectorClear(Array) \
    _VectorFieldSet(Array, 1, 0)

/**
 * @brief Gets the total current allocated capacity (Field 0) of the vector.
 */
#define VectorCapacity(Array) \
    _VectorFieldGet(Array, 0)

/**
 * @brief Gets the current number of elements stored (Field 1) in the vector.
 */
#define VectorSize(Array) \
    _VectorFieldGet(Array, 1)

/**
 * @brief Gets the byte size of a single element (Field 2 / Stride) in the vector.
 */
#define VectorStride(Array) \
    _VectorFieldGet(Array, 2)

/**
 * @brief Manually overrides the element count (Field 1) of the vector.
 */
#define VectorSizeSet(Array, Value) \
    _VectorFieldSet(Array, 1, Value)


// --- ITERATOR HELPERS ---

/**
 * @brief Returns a pointer to the start of the vector buffer.
 */
#define VectorBegin(Array) (Array)

/**
 * @brief Returns a pointer past the last element in the vector.
 * Converts pointer to `char*` to perform byte-level arithmetic (Size * Stride bytes offset),
 * then casts back to the original element pointer type (`typeof(Array)`).
 */
#define VectorEnd(Array) ((typeof(Array))((char*)(Array) + (VectorSize(Array) * VectorStride(Array))))


// --- PRIVATE INTERNAL IMPLEMENTATION FUNCTIONS ---
// Note: These functions handle the raw byte manipulation and hidden header offset calculations.
// Do NOT call directly—use the macro abstraction layer above.

/** Creates memory allocation including hidden header fields before the user pointer. */
extern void*  _VectorCreate(UInt64 Length, UInt64 Stride);

/** Releases the vector and hidden header block from heap memory. */
extern void   _VectorDestroy(void* Array);

/** Handles auto-resizing/reallocation and copies value bytes to end of array. */
extern void*  _VectorAppend(void* Array, const void* ValuePtr);

/** Handles memory shifting when removing an element by reference. */
extern void   _VectorRemove(void* Array, void* Dest);

/** Shifts existing elements right to insert value bytes at `Index`. */
extern void*  _VectorInsertAt(void* Array, UInt64 Index, void* ValuePtr);

/** Removes element at `Index`, shifts remaining elements left, and optionally copies removed data to `Dest`. */
extern void*  _VectorRemoveAt(void* Array, UInt64 Index, void* Dest);

/** Retrieves hidden header metadata attributes (0 = Capacity, 1 = Size, 2 = Stride). */
extern UInt64 _VectorFieldGet(void* Array, UInt64 Field);

/** Sets hidden header metadata attributes. */
extern void   _VectorFieldSet(void* Array, UInt64 Field, UInt64 Value);