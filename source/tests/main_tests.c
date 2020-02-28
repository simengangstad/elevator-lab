/**
 * @file
 * @brief Entry testing for module testing.
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include "../hardware.h"
#include "door_tests.h"
#include "fsm_tests.h"
#include "priority_queue_tests.h"

static void sigint_handler(int sig) {
    (void)(sig);
    printf("Terminating elevator\n");
    hardware_command_movement(HARDWARE_MOVEMENT_STOP);
    exit(0);
}

int test() {
    int error = hardware_init();
    if (error != 0) {
        fprintf(stderr, "Unable to initialize hardware\n");
        exit(1);
    }

    signal(SIGINT, sigint_handler);

    door_tests_validate();
    fsm_tests_validate();
    priority_queue_tests_validate();

    return 0;
}
