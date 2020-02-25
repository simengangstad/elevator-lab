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

static void sigint_handler(int sig) {
    (void)(sig);
    printf("Terminating elevator\n");
    hardware_command_movement(HARDWARE_MOVEMENT_STOP);
    exit(0);
}

int main() {
    int error = hardware_init();
    if (error != 0) {
        fprintf(stderr, "Unable to initialize hardware\n");
        exit(1);
    }

    signal(SIGINT, sigint_handler);

    doorTestsValidate();
    fsmTestsValidate();

    return 0;
}
