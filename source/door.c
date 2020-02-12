#include "door.h"

void doorRequestOpen() {
    doorLastCloseRequestTime = clock();
    hardware_command_door_open(1);
    doorIsCurrentlyOpen = true;
}

void doorUpdate() {

    if (doorIsOpen()) {
        if (hardware_read_obstruction_signal()) {
            doorLastCloseRequestTime = clock();
        }

        const float interval = (float)(clock() - doorLastCloseRequestTime) / CLOCKS_PER_SEC;

        if (interval >= DOOR_OPEN_TIME_INTERVAL) {
            hardware_command_door_open(0);
            doorIsCurrentlyOpen = false;
        }
    }
}

bool doorIsOpen() {
    return doorIsCurrentlyOpen;
}
