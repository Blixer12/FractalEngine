#include "Event.h"

#include "Memory.h"
#include "Containers/Vector.h"

constexpr UInt16 MaxMessageCodes = 16384;

typedef struct RegisteredEvent {
    void* Receiver;
    PFN_OnEvent Callback;
} RegisteredEvent;

typedef struct EventCodeEntry {
    RegisteredEvent* Events;
} EventCodeEntry;

typedef struct EventState {
    EventCodeEntry Registered[MaxMessageCodes];
} EventState;

static EventState* StatePtr;

void EventSystemInitialize(UInt64* MemoryRequirement, void* State) {
    *MemoryRequirement = (UInt64)sizeof(EventState);
    if (State == 0) 
    {
        return;
    }

    FMZeroMemory(State, sizeof(EventState));
    StatePtr = State;

    (void)MemoryRequirement;;
}

void EventSystemShutdown(void* State) 
{
    (void)State;
    if (StatePtr)
    {
        // Free the events arrays. And objects pointed to should be destroyed on their own.
        for(UInt16 i = 0; i < MaxMessageCodes; ++i)
        {
            if (StatePtr->Registered[i].Events != 0)
            {
                VectorDestroy(StatePtr->Registered[i].Events);
                StatePtr->Registered[i].Events = 0;
            }
        }
    }
}

Bool8 EventRegister(UInt16 Code, void* Receiver, PFN_OnEvent Callback)
{
    if (!StatePtr)
    {
        return false;
    }

    if (StatePtr->Registered[Code].Events == 0)
    {
        StatePtr->Registered[Code].Events = VectorCreate(RegisteredEvent);
    }

    UInt64 RegisteredCount = VectorSize(StatePtr->Registered[Code].Events);
    for(UInt64 i = 0; i < RegisteredCount; ++i)
    {
        if (StatePtr->Registered[Code].Events[i].Receiver == Receiver)
        {
            // TODO: warn
            return false;
        }
    }

    // If at this point, no duplicate was found. Proceed with registration.
    RegisteredEvent Event;
    Event.Receiver = Receiver;
    Event.Callback = Callback;
    VectorAppend(StatePtr->Registered[Code].Events, Event);

    return true;
}

Bool8 EventUnregister(UInt16 Code, void* Receiver, PFN_OnEvent Callback) {
    if (!StatePtr)
    {
        return false;
    }

    // On nothing is registered for the code, boot out.
    if (StatePtr->Registered[Code].Events == 0) {
        // TODO: warn
        return false;
    }

    UInt64 RegisteredCount = VectorSize(StatePtr->Registered[Code].Events);
    for(UInt64 i = 0; i < RegisteredCount; ++i) {
        RegisteredEvent Event = StatePtr->Registered[Code].Events[i];
        if (Event.Receiver == Receiver && Event.Callback == Callback) {
            // Found one, remove it
            RegisteredEvent RemovedEvent;
            VectorRemoveAt(StatePtr->Registered[Code].Events, i, &RemovedEvent);
            return true;
        }
    }

    // Not found.
    return false;
}

Bool8 EventFire(UInt16 Code, void* Sender, EventContext Context) 
{
    if (!StatePtr)
    {
        return false;
    }

    // If nothing is registered for the code, boot out.
    if (StatePtr->Registered[Code].Events == 0)
    {
        return false;
    }

    UInt64 RegisteredCount = VectorSize(StatePtr->Registered[Code].Events);
    for (UInt64 i = 0; i < RegisteredCount; ++i)
    {
        RegisteredEvent Event = StatePtr->Registered[Code].Events[i];
        if (Event.Callback(Code, Sender, Event.Receiver, Context))
        {
            // Message has been handled, does not send other listeners.
            return true;
        }
    }

    // Event finished propagation without being explicitly handled by any listener
    return false;
}