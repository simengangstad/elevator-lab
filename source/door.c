/**
 * @file
 * @brief Implementation of Door.
 */

#include "door.h"

/**
 * @brief Specifies how long the door will be open if no obstruction happens.
 */
static const float DOOR_OPEN_TIME_INTERVAL = 3.0f;

/**
 * @brief Keeps track of the time since we last requested the door to close.
 * 
 * @note This time will be reset to the current time if there occurs an obstruction.
 */
static time_t globalDoorLastCloseRequestTime;

/**
 * @brief Tracks whether the door is open or not. 
 */
static bool globalDoorIsCurrentlyOpen = false;

void doorRequestOpenAndAutoclose() {
    globalDoorLastCloseRequestTime = time(NULL);
    hardware_command_door_open(1);
    globalDoorIsCurrentlyOpen = true;
}

void doorUpdate() {

    if (doorIsOpen()) {
        if (hardware_read_obstruction_signal()) {
            globalDoorLastCloseRequestTime = time(NULL);
        }

        const time_t interval = time(NULL) - globalDoorLastCloseRequestTime;

        if (interval >= DOOR_OPEN_TIME_INTERVAL) {
            hardware_command_door_open(0);
            globalDoorIsCurrentlyOpen = false;
        }
    }
}

bool doorIsOpen() {
    return globalDoorIsCurrentlyOpen;
}
