/**
 * @file 
 * 
 * @brief Implementation of the unit tests checks.
 */

#include "unit_tests.h"

/**
 * @brief Handles interrupts from the command line.
 * 
 * @param sig The signal. 
 */
static void sigint_handler(int sig) {
    (void)(sig);
    printf("Terminating elevator\n");
    hardware_command_movement(HARDWARE_MOVEMENT_STOP);
    exit(0);
}

void unit_tests_check() {
    int error = hardware_init();
    if (error != 0) {
        fprintf(stderr, "Unable to initialize hardware\n");
        exit(1);
    }

    signal(SIGINT, sigint_handler);

    door_tests_validate();
    fsm_tests_validate();
    priority_queue_tests_validate();
}