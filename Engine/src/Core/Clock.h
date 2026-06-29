#pragma once

#include "Defines.h"

typedef struct Clock
{
    Float64 StartTime;
    Float64 Elapsed;
} Clock;

void ClockUpdate(Clock* Clock);
void ClockStart(Clock* Clock);
void ClockStop(Clock* Clock);