/**
 * @file 
 * @brief Represents the actions with the door. It contains its own timer (thus tracks state). It also calls to hardware.
 */

#ifndef DOOR_H
#define DOOR_H
#define DOOR_OPEN_TIME_INTERVAL 3.0

#include <stdbool.h>
#include <time.h>
#include "hardware.h"

/**
 * @brief Will open the door and close it after a number of seconds specified 
 *        by #DOOR_OPEN_TIME_INTERVAL.
 */
void door_request_open_and_autoclose();

/**
 * @brief Updates the timer and checks whether we should close the door (which happens 
 *        after the time interval specified by #DOOR_OPEN_TIME_INTERVAL).
 * 
 * @note If there is an obstruction the timer will be reset and the door will try to close
 *       again after #DOOR_OPEN_TIME_INTERVAL.
 */
void door_update();

/**
 * @return Whether the door is open.
 */
bool door_is_open();

#endif
