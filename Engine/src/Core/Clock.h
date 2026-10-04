#pragma once

#include "Defines.h"

typedef struct Clock
{
    Float64 StartTime;
    Float64 Elapsed;
} Clock;

FAPI void ClockUpdate(Clock* Clock);
FAPI void ClockStart(Clock* Clock);
FAPI void ClockStop(Clock* Clock);