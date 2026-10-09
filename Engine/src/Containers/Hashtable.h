#pragma once

#include "Defines.h"

/**
 * @brief Represents a simple Hashtable. Members of this structure
 * should not be modified outside the functions associated with it.
 * 
 * For non-pointer types, table retains a copy of the value. For 
 * pointer types, make sure to use the Ptr setter and getter. Table
 * does not take ownership of pointers or associated memory allocations,
 * and should be managed externally.
 */
typedef struct Hashtable {
    UInt64 ElementSize;
    UInt32 ElementCount;
    Bool8 IsPointerType;
    void* Memory;
} Hashtable;

/**
 * @brief Creates a Hashtable and stores it in out_Hashtable.
 * 
 * @param ElementSize The size of each element in bytes.
 * @param ElementCount The maximum number of elements. Cannot be resized.
 * @param Memory A block of memory to be used. Should be equal in size to ElementSize * ElementCount;
 * @param IsPointerType Indicates if this Hashtable will hold pointer types.
 * @param Hashtable A pointer to a Hashtable in which to hold relevant data.
 */
FAPI void HashtableCreate(UInt64 ElementSize, UInt32 ElementCount, void* Memory, Bool8 IsPointerType, Hashtable* Hashtable);

/**
 * @brief Destroys the provided Hashtable. Does not release memory for pointer types.
 * 
 * @param Table A pointer to the table to be destroyed.
 */
FAPI void HashtableDestroy(Hashtable* Table);

/**
 * @brief Stores a copy of the data in value in the provided Hashtable. 
 * Only use for tables which were *NOT* created with is_pointer_type = true.
 * 
 * @param Table A pointer to the table to get from. Required.
 * @param Name The name of the entry to set. Required.
 * @param Value The value to be set. Required.
 * @return True, or false if a null pointer is passed.
 */
FAPI Bool8 HashtableSet(Hashtable* Table, const char* Name, void* Value);

/**
 * @brief Stores a pointer as provided in value in the Hashtable.
 * Only use for tables which were created with is_pointer_type = true.
 * 
 * @param Table A pointer to the table to get from. Required.
 * @param Name The name of the entry to set. Required.
 * @param Value A pointer value to be set. Can pass 0 to 'unset' an entry.
 * @return True; or false if a null pointer is passed or if the entry is 0.
 */
FAPI Bool8 HashtableSetPtr(Hashtable* Table, const char* Name, void** Value);

/**
 * @brief Obtains a copy of data present in the Hashtable.
 * Only use for tables which were *NOT* created with is_pointer_type = true.
 * 
 * @param Table A pointer to the table to retrieved from. Required.
 * @param Name The name of the entry to retrieved. Required.
 * @param Value A pointer to store the retrieved value. Required.
 * @return True; or false if a null pointer is passed.
 */
FAPI Bool8 HashtableGet(Hashtable* Table, const char* Name, void* Value);

/**
 * @brief Obtains a pointer to data present in the Hashtable.
 * Only use for tables which were created with is_pointer_type = true.
 * 
 * @param Table A pointer to the table to retrieved from. Required.
 * @param Name The name of the entry to retrieved. Required.
 * @param Value A pointer to store the retrieved value. Required.
 * @return True if retrieved successfully; false if a null pointer is passed or is the retrieved value is 0.
 */
FAPI Bool8 HashtableGetPtr(Hashtable* Table, const char* Name, void** Value);

/**
 * @brief Fills all entries in the Hashtable with the given value.
 * Useful when non-existent names should return some default value.
 * Should not be used with pointer table types.
 * 
 * @param Table A pointer to the table filled. Required.
 * @param Value The value to be filled with. Required.
 * @return True if successful; otherwise false.
 */
FAPI Bool8 HashtableFill(Hashtable* Table, void* Value);