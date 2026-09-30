#include "Event.h"

#include "Memory.h"
#include "Containers/Vector.h"

#define MAX_MESSAGE_CODES 16384

typedef struct RegisteredEvent {
    void* Receiver;
    PFN_OnEvent Callback;
} RegisteredEvent;

typedef struct EventCodeEntry {
    RegisteredEvent* Events;
} EventCodeEntry;

typedef struct EventSystemState {
    EventCodeEntry Registered[MAX_MESSAGE_CODES];
} EventSystemState;

static Bool8 IsInitialized;
static EventSystemState State;

Bool8 EventSystemInitialize() {
    if (IsInitialized == true) {
        return false;
    }
    IsInitialized = false;
    FMZeroMemory(&State, sizeof(State));

    IsInitialized = true;

    return true;
}

void EventSystemShutdown() 
{
    // Free the events arrays. And objects pointed to should be destroyed on their own.
    for(UInt16 i = 0; i < MAX_MESSAGE_CODES; ++i)
    {
        if (State.Registered[i].Events != 0)
        {
            VectorDestroy(State.Registered[i].Events);
            State.Registered[i].Events = 0;
        }
    }
}

Bool8 EventRegister(UInt16 Code, void* Receiver, PFN_OnEvent Callback)
{
    if (IsInitialized == false)
    {
        return false;
    }

    if (State.Registered[Code].Events == 0)
    {
        State.Registered[Code].Events = VectorCreate(RegisteredEvent);
    }

    UInt64 RegisteredCount = VectorSize(State.Registered[Code].Events);
    for(UInt64 i = 0; i < RegisteredCount; ++i)
    {
        if (State.Registered[Code].Events[i].Receiver == Receiver)
        {
            // TODO: warn
            return false;
        }
    }

    // If at this point, no duplicate was found. Proceed with registration.
    RegisteredEvent Event;
    Event.Receiver = Receiver;
    Event.Callback = Callback;
    VectorAppend(State.Registered[Code].Events, Event);

    return true;
}

Bool8 EventUnregister(UInt16 Code, void* Receiver, PFN_OnEvent Callback) {
    if (IsInitialized == false)
    {
        return false;
    }

    // On nothing is registered for the code, boot out.
    if (State.Registered[Code].Events == 0) {
        // TODO: warn
        return false;
    }

    UInt64 RegisteredCount = VectorSize(State.Registered[Code].Events);
    for(UInt64 i = 0; i < RegisteredCount; ++i) {
        RegisteredEvent Event = State.Registered[Code].Events[i];
        if (Event.Receiver == Receiver && Event.Callback == Callback) {
            // Found one, remove it
            RegisteredEvent RemovedEvent;
            VectorRemoveAt(State.Registered[Code].Events, i, &RemovedEvent);
            return true;
        }
    }

    // Not found.
    return false;
}

Bool8 EventFire(UInt16 Code, void* Sender, EventContext Context) 
{
    if (IsInitialized == false)
    {
        return false;
    }

    // If nothing is registered for the code, boot out.
    if (State.Registered[Code].Events == 0)
    {
        return false;
    }

    UInt64 RegisteredCount = VectorSize(State.Registered[Code].Events);
    for (UInt64 i = 0; i < RegisteredCount; ++i)
    {
        RegisteredEvent Event = State.Registered[Code].Events[i];
        if (Event.Callback(Code, Sender, Event.Receiver, Context))
        {
            // Message has been handled, does not send other listeners.
            return true;
        }
    }

    // Event finished propagation without being explicitly handled by any listener
    return false;
}