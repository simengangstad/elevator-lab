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
static clock_t globalDoorLastCloseRequestTime;

/**
 * @brief Tracks whether the door is open or not. 
 */
static bool globalDoorIsCurrentlyOpen = false;

void doorRequestOpenAndAutoclose() {
    globalDdoorLastCloseRequestTime = clock();
    hardware_command_door_open(1);
    globalDoorIsCurrentlyOpen = true;
}

void doorUpdate() {

    if (doorIsOpen()) {
        if (hardware_read_obstruction_signal()) {
            globalDoorLastCloseRequestTime = clock();
        }

        const float interval = (float)(clock() - globalDoorLastCloseRequestTime) / CLOCKS_PER_SEC;

        if (interval >= DOOR_OPEN_TIME_INTERVAL) {
            hardware_command_door_open(0);
            globalDoorIsCurrentlyOpen = false;
        }
    }
}

bool doorIsOpen() {
    return globalDoorIsCurrentlyOpen;
}
