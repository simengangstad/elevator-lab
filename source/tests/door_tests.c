/**
 * @file
 * @brief Implementation of the tests for the door module.
 */

#include "door_tests.h"

/**
 * @brief Will check if the door does not close when there is an obstruction. 
 *
 * @note Test TDOOR-1.
 *
 * @return True if the door did not close as there were an obstruction, false
 * 		   if not.
 */
static bool door_tests_check_keeps_open_while_obstruction() {
    const float duration_to_check = 5.0f;

    printf("Enable obstruction now please. Will open door and try to close for %f seconds. Press enter to continue...\n", duration_to_check);
    test_util_wait_until_enter_key_is_pressed();
    door_request_open_and_autoclose();

    const time_t start_time = time(NULL);
    bool door_open = false;

    while (time(NULL) - start_time <= duration_to_check) {
        door_update();

        door_open = door_is_open();

        if (!door_open) {
            break;
        }

        sleep(1);
    }

    return door_open;
}

/**
 * @brief Will check if the door closes when there is NOT an obstruction. This function will also implicitely check if
 *        the timer for the door gets setup correctly, updates and that the door closes after 3 seconds.
 *
 * @note Test TDOOR-2, TDOOR-3, TDOOR-4, TDOOR-6
 *
 * @return True if the door did close, false if not.
 */
static bool door_tests_check_closes_when_no_obstruction() {
    printf("Closing door...\n");
    hardware_command_door_open(0);

    const float duration_to_check = 3.5f;

    printf("Disable obstruction now please. Will open door and try to autoclose. Press enter to continue...\n");
    test_util_wait_until_enter_key_is_pressed();

    door_request_open_and_autoclose();

    const time_t start_time = time(NULL);

    while (time(NULL) - start_time <= duration_to_check) {
        door_update();
        sleep(1);
    }

    return !door_is_open();
}

/**
 * @brief Checks if the timer gets reset when then the obstruction signal goes high.
 *
 * @note Test TDOOR-5
 */
static void door_tests_check_timer_reset() {
    printf("Closing door...\n");
    hardware_command_door_open(0);

    printf("Enable obstruction now please. Will open door and try to autoclose. Disable obstruction signal when ready and count the seconds it takes for the door to close. Press enter to continue...\n");
    test_util_wait_until_enter_key_is_pressed();
    door_request_open_and_autoclose();

    while (door_is_open()) {
        door_update();
        sleep(1);
    }
}

/**
 * @brief Checks if the the door module returns the right value for when the door is open. 
 *
 * @note Test TDOOR-7 
 *
 * @return True if the function for checking if the door is open returned the right value, false if not.
 */
static bool door_tests_check_door_is_open_function() {
    const float duration_to_check = 3.5f;

    printf("Disable obstruction please. Will open door and try to autoclose. Press enter to continue...\n");
    test_util_wait_until_enter_key_is_pressed();

    door_request_open_and_autoclose();
    const bool door_opened = door_is_open();

    const time_t start_time = time(NULL);

    while (time(NULL) - start_time <= duration_to_check) {
        door_update();
        sleep(1);
    }

    const bool door_closed = !door_is_open();

    return door_opened && door_closed;
}

void door_test_validate() {
    printf("=========== Starting door tests ===========\n\n");
    printf("Moving elevator to floor...\n\n");

    while (1) {
        if (hardware_read_floor_sensor(0) ||
            hardware_read_floor_sensor(1) ||
            hardware_read_floor_sensor(2) ||
            hardware_read_floor_sensor(3)) {
            hardware_command_movement(HARDWARE_MOVEMENT_STOP);
            break;
        } else {
            hardware_command_movement(HARDWARE_MOVEMENT_DOWN);
        }
    }

    printf("1. Check if door keeps open when there is an obstruction (TDOOR-1)\n");
    assert(door_tests_check_keeps_open_while_obstruction());
    printf("1. Passed\n\n");

    printf("2. Check if door closes when there is no obstruction (TDOOR-2, TDOOR-3, TDOOR-4, TDOOR-6)\n");
    assert(door_tests_check_closes_when_no_obstruction());
    printf("2. Passed\n\n");

    printf("3. Check if the timer resets. Validate visually (TDOOR-5)\n");
    door_tests_check_timer_reset();
    printf("3. End\n\n");

    printf("4. Will check if the function for whether the door is open returns the right value for one call to open and close door (TDOOR-7).\n");
    assert(door_tests_check_door_is_open_function());
    printf("4. Passed\n\n");
    printf("=========== Door tests passed ===========\n\n");
}
