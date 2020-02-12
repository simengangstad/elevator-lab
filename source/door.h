/**
 * @file door.h
 * @brief Represents the actions with the door. It contains its own timer (thus tracks state). It also calls to hardware.
 */ 

#ifndef DOOR_H
#define DOOR_H

#include <time.h>
#include <stdbool.h>
#include "hardware.h"

/**
 * @brief Specifies how long the door will be open if no obstruction happens.
 */
static const float DOOR_OPEN_TIME_INTERVAL = 3.0f;

/**
 * @brief Keeps track of the time since we last requested the door to close.
 * 
 * @note This time will be reset to the current time if there occurs an obstruction.
 */
static clock_t doorLastCloseRequestTime;

/**
 * @brief Tracks whether the door is open or not. 
 */
static bool doorIsCurrentlyOpen;

/**
 * @brief Will open the door and close it after a number of seconds specified 
 *        by #DOOR_OPEN_TIME_INTERVAL.
 */
void doorRequestOpen();

/**
 * @brief Updates the timer and checks whether we should close the door (which happens 
 *        after the time interval specified by #DOOR_OPEN_TIME_INTERVAL).
 */
void doorUpdate();

/**
 * @return Whether the door is open.
 */
bool doorIsOpen();

#endif