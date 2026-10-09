#include "Hashtable.h"

#include "Core/Memory.h"
#include "Core/Logger.h"

UInt64 HashName(const char* Name, UInt32 ElementCount) 
{
    if (!Name || ElementCount == 0) {
        return 0;
    }

    // Multiplier used when generating Hashes! chose because it is prime, and equals 2^7 - 1
    constexpr UInt64 Multiplier = 127;

    unsigned const char* UnsignedString;
    UInt64 Hash = 0;

    for (UnsignedString = (unsigned const char*)Name; *UnsignedString; UnsignedString++) {
        Hash = Hash * Multiplier + *UnsignedString;
    }

    // Mod it against the size of the table.
    Hash %= ElementCount;

    return Hash;
}

void HashtableCreate(UInt64 ElementSize, UInt32 ElementCount, void* Memory, Bool8 IsPointerType, Hashtable* Hashtable)
{
    if (!Memory || !Hashtable) {
        FLERROR("HashtableCreate failed! Pointer to Memory and the ouputted Hashtable are required.");
        return;
    }
    if (!ElementCount || !ElementSize) {
        FLERROR("ElementSize and ElementCount must be a positive non-zero value.");
        return;
    }

    // TODO: Might want to require an allocator and allocate this memory instead.
    Hashtable->Memory = Memory;
    Hashtable->ElementCount = ElementCount;
    Hashtable->ElementSize = ElementSize;
    Hashtable->IsPointerType = IsPointerType;
    FMZeroMemory(Hashtable->Memory, ElementSize * ElementCount);
}

void HashtableDestroy(Hashtable* Table)
{
    if (Table) {
        // TODO: If using allocator above, free memory here.
        FMZeroMemory(Table, sizeof(Hashtable));
    }
}

Bool8 HashtableSet(Hashtable* Table, const char* Name, void* Value)
{
     if (!Table || !Name || !Value) {
        FLERROR("HashtableSet requires Table, Name and Value to exist.");
        return false;
    }
    if (Table->IsPointerType) {
        FLERROR("HashtableSet should not be used with tables that have pointer types. Use HashtableSetPtr instead.");
        return false;
    }

    UInt64 Hash = HashName(Name, Table->ElementCount);
    FMCopyMemory(Table->Memory + (Table->ElementSize * Hash), Value, Table->ElementSize);
    return true;
}

Bool8 HashtableSetPtr(Hashtable* Table, const char* Name, void** Value)
{
    if (!Table || !Name) {
        FLWARN("HashtableSetPtr requires Table and Name to exist.");
        return false;
    }
    if (!Table->IsPointerType) {
        FLERROR("HashtableSetPtr should not be used with tables that do not have pointer types. Use HashtableSet instead.");
        return false;
    }

    UInt64 Hash = HashName(Name, Table->ElementCount);
    ((void**)Table->Memory)[Hash] = Value ? *Value : 0;
    return true;
}

Bool8 HashtableGet(Hashtable* Table, const char* Name, void* Value)
{
    if (!Table || !Name || !Value) {
        FLWARN("hashtable_get requires Table, Name and Value to exist.");
        return false;
    }
    if (Table->IsPointerType) {
        FLERROR("HashtableGet should not be used with tables that have pointer types. Use HashtableSetPtr instead.");
        return false;
    }
    UInt64 Hash = HashName(Name, Table->ElementCount);
    FMCopyMemory(Value, Table->Memory + (Table->ElementSize * Hash), Table->ElementSize);
    return true;
}

Bool8 HashtableGetPtr(Hashtable* Table, const char* Name, void** Value)
{
    if (!Table || !Name || !Value) {
        FLWARN("HashtableGetPtr requires Table, Name and Value to exist.");
        return false;
    }
    if (!Table->IsPointerType) {
        FLERROR("HashtableGetPtr should not be used with tables that do not have pointer types. Use HashtableGet instead.");
        return false;
    }

    UInt64 Hash = HashName(Name, Table->ElementCount);
    *Value = ((void**)Table->Memory)[Hash];
    return *Value != 0;
}

Bool8 HashtableFill(Hashtable* Table, void* Value)
{
     if (!Table || !Value) {
        FLWARN("HashtableFill requires Table and Value to exist.");
        return false;
    }
    if (Table->IsPointerType) {
        FLERROR("HashtableFill should not be used with tables that have pointer types.");
        return false;
    }

    for (UInt32 i = 0; i < Table->ElementCount; ++i) {
        FMCopyMemory(Table->Memory + (Table->ElementSize * i), Value, Table->ElementSize);
    }

    return true;
}