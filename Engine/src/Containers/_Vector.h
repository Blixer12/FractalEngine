/**
 * @file _Vector.h
 * @author Blixer12
 * @brief This is a private header that should NEVER be included anywhere else
 * 
 * @version 1.0
 * @date 2026-10-11
 * 
 * @copyright Fractal Engine is Copyright (c) Blixer12 2026
 * 
 */

#pragma once

#include "Defines.h"

enum {
    VECTOR_CAPACITY,
    VECTOR_SIZE,
    VECTOR_STRIDE,
    FIELD_LENGTH
};

/**
 * @brief Creates a new Vector of the given length and stride.
 * Note that this performs a dynamic memory allocation. 
 * @note Avoid using this directly; use the VectorCreate macro instead, this is a private function, as denoted from the underscore (_)
 * @param Length The default number of elements in the array.
 * @param Stride The size of each array element.
 * @returns A pointer representing the block of memory containing the array.
 */
FAPI void* _VectorCreate(UInt64 Length, UInt64 Stride);

/**
 * @brief destroys the given Vector, freeing resources. Frees associated memory.
 * @note Avoid using this function directly. Use the VectorDestroy macro instead.
 * @param Array The Vector to be destroyed.
 */
FAPI void _VectorDestroy(void* Array);


/**
 * @brief Returns the value for the given field.
 * @note This is an internal feature, use public macros instead such as VectorSize.
 * @param Array The array to retrieve the field for.
 * @param Field The index of the field to be retrieved.
 * @returns The value for the given field.
 */
FAPI UInt64 _VectorFieldGet(void* Array, UInt64 Field);

/**
 * @brief Sets the value for the given field.
 * @note This is an internal feature and should NEVER be called directly.
 * @param Array The array to set the field for.
 * @param Field The index of the field to be set.
 * @param Value The value to be set for the given field.
 */
FAPI void _VectorFieldSet(void* Array, UInt64 Field, UInt64 Value);

/**
 * @brief Resizes the given array using internal resizing amounts.
 * Causes a new allocation.
 * @note This is an internal implementation detail and should NEVER be called directly.
 * @param Array The array to be resized.
 * @returns A pointer to the resized array block.
 */
FAPI void* _VectorResize(void* Array);

/**
 * @brief Pushes a new entry to the given array. Resizes if necessary.
 * @note Avoid using this directly; call the VectorAppend macro instead.
 * @param Array The array to be Appended to.
 * @param ValuePtr A pointer to the value to be Appended. A copy of this value is taken.
 * @returns A pointer to the array block.
 */
FAPI void* _VectorAppend(void* Array, const void* ValuePtr);

/**
 * @brief Pops an entry out of the array and places it into dest.
 * @note Avoid using this directly; call the VectorRemove macro instead.
 * @param Array The array to pop from.
 * @param Dest A pointer to hold the popped value.
 */
FAPI void _VectorRemove(void* Array, void* Dest);


/**
 * @brief Pops an entry out of the array at the given index and places it into dest.
 * Brings in all entries after the popped index in by one.
 * @note Avoid using this directly; call the VectorRemoveAt macro instead.
 * @param Array The array to pop from.
 * @param Index The index to pop from.
 * @param Dest A pointer to hold the popped value.
 * @returns The array block.
 */
FAPI void* _VectorRemoveAt(void* Array, UInt64 Index, void* Dest);

/**
 * @brief Inserts a copy of the given value into the supplied array at the given index.
 * Triggers an array resize if required.
 * @note Avoid using this directly; call the VectorInsertAt macro instead.
 * @param Array The array to insert into.
 * @param Index The index to insert at.
 * @param ValuePtr A pointer holding the value to be inserted.
 * @returns The array block.
 */
FAPI void* _VectorInsertAt(void* Array, UInt64 Index, void* ValuePtr);