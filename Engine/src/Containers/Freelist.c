#include "Freelist.h"

#include "Core/Memory.h"
#include "Core/Logger.h"

typedef struct FreelistNode {
    UInt64 Offset;
    UInt64 Size;
    struct FreelistNode* Next;
} FreelistNode;

typedef struct InternalState {
    UInt64 TotalSize;
    UInt64 MaxEntries;
    FreelistNode* Head;
    FreelistNode* Nodes;
} InternalState;

FreelistNode* GetNode(Freelist* List);
void ReturnNode(Freelist* List, FreelistNode* Node);

void FreelistCreate(UInt64 TotalSize, UInt64* MemoryRequirement, void* Memory, Freelist* OutList) {
    // FIX: Prevent the metadata explosion by capping MaxEntries to a safe maximum limit.
    // 32,768 entries is more than enough to track fragmentation for large Memory pools.
    UInt64 MaxEntries = TotalSize / 64; 
    if (MaxEntries < 512) {
        MaxEntries = 512;
    }
    if (MaxEntries > 32768) {
        MaxEntries = 32768; // Hard cap! Never let tracking Nodes spiral out of control.
    }

    *MemoryRequirement = sizeof(InternalState) + (sizeof(FreelistNode) * MaxEntries);
    if (!Memory) {
        return;
    }

    UInt64 MemMin = (sizeof(InternalState) + sizeof(FreelistNode)) * 8;
    if (TotalSize < MemMin) {
        FLWARN(
            "Freelists are very inefficient with amounts of Memory less than %iB; it is recommended to not use this structure in this case.",
            MemMin);
    }

    OutList->Memory = Memory;

    FMZeroMemory(OutList->Memory, *MemoryRequirement);
    InternalState* State = OutList->Memory;
    State->Nodes = (void*)(OutList->Memory + sizeof(InternalState));
    State->MaxEntries = MaxEntries;
    State->TotalSize = TotalSize;

    State->Head = &State->Nodes[0];
    State->Head->Offset = 0;
    State->Head->Size = TotalSize;
    State->Head->Next = 0;

    for (UInt64 I = 1; I < State->MaxEntries; ++I) {
        State->Nodes[I].Offset = InvalidID;
        State->Nodes[I].Size = InvalidID;
    }
}

void FreelistDestroy(Freelist* List) {
    if (List && List->Memory) {
        InternalState* State = List->Memory;
        FMZeroMemory(List->Memory, sizeof(InternalState) + sizeof(FreelistNode) * State->MaxEntries);
        List->Memory = 0;
    }
}

Bool8 FreelistAllocateBlock(Freelist* List, UInt64 Size, UInt64* OutOffset) {
    if (!List || !OutOffset || !List->Memory) {
        return false;
    }
    InternalState* State = List->Memory;
    FreelistNode* Node = State->Head;
    FreelistNode* Previous = 0;

    while (Node) {
        if (Node->Size == Size) {
            *OutOffset = Node->Offset;
            FreelistNode* NodeToReturn = 0;
            if (Previous) {
                Previous->Next = Node->Next;
                NodeToReturn = Node;
            } else {
                NodeToReturn = State->Head;
                State->Head = Node->Next;
            }
            ReturnNode(List, NodeToReturn);
            return true;
        } else if (Node->Size > Size) {
            *OutOffset = Node->Offset;
            Node->Size -= Size;
            Node->Offset += Size;
            return true;
        }

        Previous = Node;
        Node = Node->Next;
    }

    UInt64 FreeSpace = FreelistFreeSpace(List);
    FLWARN("FreelistFindBlock, no block with enough free space found (requested: %uB, available: %lluB).", Size, FreeSpace);
    return false;
}

Bool8 FreelistFreeBlock(Freelist* List, UInt64 Size, UInt64 Offset) {
    if (!List || !List->Memory || !Size) {
        return false;
    }
    InternalState* State = List->Memory;
    FreelistNode* Node = State->Head;
    FreelistNode* Previous = 0;

    if (!Node) {
        FreelistNode* NewNode = GetNode(List);
        NewNode->Offset = Offset;
        NewNode->Size = Size;
        NewNode->Next = 0;
        State->Head = NewNode;
        return true;
    } else {
        while (Node) {
            if (Node->Offset == Offset) {
                Node->Size += Size;

                if (Node->Next && Node->Next->Offset == Node->Offset + Node->Size) {
                    Node->Size += Node->Next->Size;
                    FreelistNode* Next = Node->Next;
                    Node->Next = Node->Next->Next;
                    ReturnNode(List, Next);
                }
                return true;
            } else if (Node->Offset > Offset) {
                FreelistNode* NewNode = GetNode(List);
                NewNode->Offset = Offset;
                NewNode->Size = Size;

                if (Previous) {
                    Previous->Next = NewNode;
                    NewNode->Next = Node;
                } else {
                    NewNode->Next = Node;
                    State->Head = NewNode;
                }

                if (NewNode->Next && NewNode->Offset + NewNode->Size == NewNode->Next->Offset) {
                    NewNode->Size += NewNode->Next->Size;
                    FreelistNode* Rubbish = NewNode->Next;
                    NewNode->Next = Rubbish->Next;
                    ReturnNode(List, Rubbish);
                }

                if (Previous && Previous->Offset + Previous->Size == NewNode->Offset) {
                    Previous->Size += NewNode->Size;
                    FreelistNode* Rubbish = NewNode;
                    Previous->Next = Rubbish->Next;
                    ReturnNode(List, Rubbish);
                }

                return true;
            }

            Previous = Node;
            Node = Node->Next;
        }
    }

    FLWARN("Unable to find block to be freed. Corruption possible?");
    return false;
}

Bool8 FreelistResize(Freelist* List, UInt64* MemoryRequirement, void* NewMemory, UInt64 NewSize, void** OldMemory)
{
    if (!List || !MemoryRequirement || ((InternalState*)List->Memory)->TotalSize > NewSize) {
        return false;
    }

    // Enough space to hold State, plus array for all Nodes.
    UInt64 MaxEntries = (NewSize / sizeof(void*) * sizeof(FreelistNode));  //NOTE: This might have a remainder, but that's ok.
    *MemoryRequirement = sizeof(InternalState) + (sizeof(FreelistNode) * MaxEntries);
    if (!NewMemory) {
        return true;
    }

    // Assign the old Memory pointer so it can be freed.
    *OldMemory = List->Memory;

    // Copy over the old State to the new.
    InternalState* OldState = (InternalState*)List->Memory;
    UInt64 SizeDifference = NewSize - OldState->TotalSize;

    // Setup the new Memory
    List->Memory = NewMemory;

    // The block's layout is Head* first, then array of available Nodes.
    FMZeroMemory(List->Memory, *MemoryRequirement);

    // Setup the new State.
    InternalState* State = (InternalState*)List->Memory;
    State->Nodes = (void*)(List->Memory + sizeof(InternalState));
    State->MaxEntries = MaxEntries;
    State->TotalSize = NewSize;

    // Invalidate the Offset and Size for all but the first node. The invalid
    // value will be checked for when seeking a new node from the List.
    for (UInt64 i = 1; i < State->MaxEntries; ++i) {
        State->Nodes[i].Offset = InvalidID;
        State->Nodes[i].Size = InvalidID;
    }

    State->Head = &State->Nodes[0];

    // Copy over the Nodes.
    FreelistNode* NewListNode = State->Head;
    FreelistNode* OldNode = OldState->Head;
    if (!OldNode) {
        // If there is no Head, then the entire List is allocated. In this case,
        // the Head should be set to the difference of the space now available, and
        // at the end of the List.
        State->Head->Offset = OldState->TotalSize;
        State->Head->Size = SizeDifference;
        State->Head->Next = 0;
    } else {
        // Iterate the old Nodes.
        while (OldNode) {
            // Get a new node, copy the Offset/Size, and set Next to it.
            FreelistNode* NewNode = GetNode(List);
            NewNode->Offset = OldNode->Offset;
            NewNode->Size = OldNode->Size;
            NewNode->Next = 0;
            NewListNode->Next = NewNode;
            // Move to the Next entry.
            NewListNode = NewListNode->Next;

            if (OldNode->Next) {
                // If there is another node, move on.
                OldNode = OldNode->Next;
            } else {
                // Reached the end of the List.
                // Check if it extends to the end of the block. If so,
                // just append to the Size. Otherwise, create a new node and
                // attach to it.
                if (OldNode->Offset + OldNode->Size == OldState->TotalSize) {
                    NewNode->Size += SizeDifference;
                } else {
                    FreelistNode* NewNodeEnd = GetNode(List);
                    NewNodeEnd->Offset = OldState->TotalSize;
                    NewNodeEnd->Size = SizeDifference;
                    NewNodeEnd->Next = 0;
                    NewNode->Next = NewNodeEnd;
                }
                break;
            }
        }
    }

    return true;
}

void FreelistClear(Freelist* List) {
    if (!List || !List->Memory) {
        return;
    }

    InternalState* State = List->Memory;
    for (UInt64 I = 1; I < State->MaxEntries; ++I) {
        State->Nodes[I].Offset = InvalidID;
        State->Nodes[I].Size = InvalidID;
    }

    State->Head->Offset = 0;
    State->Head->Size = State->TotalSize;
    State->Head->Next = 0;
}

UInt64 FreelistFreeSpace(Freelist* List) {
    if (!List || !List->Memory) {
        return 0;
    }

    UInt64 RunningTotal = 0;
    InternalState* State = List->Memory;
    FreelistNode* Node = State->Head;
    while (Node) {
        RunningTotal += Node->Size;
        Node = Node->Next;
    }

    return RunningTotal;
}

FreelistNode* GetNode(Freelist* List) {
    InternalState* State = List->Memory;
    for (UInt64 I = 1; I < State->MaxEntries; ++I) {
        if (State->Nodes[I].Offset == InvalidID) {
            return &State->Nodes[I];
        }
    }

    return 0;
}

void ReturnNode(Freelist* List, FreelistNode* Node) {
    (void)List;
    Node->Offset = InvalidID;
    Node->Size = InvalidID;
    Node->Next = 0;
}