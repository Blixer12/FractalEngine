#include "Clock.h"

#include "Platform/Platform.h"

void ClockUpdate(Clock* Clock)
{
    if (Clock->StartTime != 0)
    {
        Clock->Elapsed = PlatformGetAbsoluteTime() - Clock->StartTime;
    }
}

void ClockStart(Clock* Clock)
{
    Clock->StartTime = PlatformGetAbsoluteTime();
    Clock->Elapsed = 0;
}

void ClockStop(Clock* Clock)
{
    Clock->StartTime = 0;
}