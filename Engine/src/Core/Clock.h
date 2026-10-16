/**
 * @file Clock.h
 * @author Blixer12
 * @brief This file contains structures and functions for the 
 * engine's clock.
 * 
 * @version 1.0
 * @date 2026-10-11
 * 
 * @copyright Fractal Engine is Copyright (c) Blixer12 2026
 * 
 */

#pragma once

#include "Defines.h"

/**
 * @brief Represents a basic clock, which can be used to track time
 * deltas in the system.
 */
typedef struct Clock
{
    /** @brief The start time of the clock. If never started, this is 0. */
    Float64 StartTime;
    /** @brief The amount of time in seconds that have elapsed since this
     * clock was started. Only accurate after a call to clock_update. 
     */
    Float64 Elapsed;
} Clock;

/** 
 * @brief Updates the provided clock. Should be called just before checking elapsed time.
 * Has no effect on non-started clocks.
 * @param Clock A pointer to the clock to be updated.
 */
FAPI void ClockUpdate(Clock* Clock);

/** 
 * @brief Starts the provided clock. Resets elapsed time.
 * @param Clock A pointer to the clock to be started.
 */
FAPI void ClockStart(Clock* Clock);

/** 
 * @brief Stops the provided clock. Does not reset elapsed time.
 * @param Clock A pointer to the clock to be stopped.
 */
FAPI void ClockStop(Clock* Clock);