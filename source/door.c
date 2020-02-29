/**
 * @file
 * @brief Implementation of Door.
 */

#include "door.h"

/**
 * @brief Keeps track of the time since we last requested the door to open and autoclose.
 * 
 * @note This time will be reset to the current time if there occurs an obstruction.
 */
static time_t m_door_last_open_and_autoclose_request_time;

/**
 * @brief Tracks whether the door is open or not. 
 */
static bool m_door_is_currenty_open = false;

void door_request_open_and_autoclose() {
    m_door_last_open_and_autoclose_request_time = time(NULL);
    hardware_command_door_open(1);
    m_door_is_currenty_open = true;
}

void door_update() {
    if (door_is_open()) {
        if (hardware_read_obstruction_signal()) {
            m_door_last_open_and_autoclose_request_time = time(NULL);
        }

        const time_t interval = time(NULL) - m_door_last_open_and_autoclose_request_time;

        if (interval >= DOOR_OPEN_TIME_INTERVAL) {
            hardware_command_door_open(0);
            m_door_is_currenty_open = false;
        }
    }
}

bool door_is_open() {
    return m_door_is_currenty_open;
}
