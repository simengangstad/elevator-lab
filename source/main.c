/*! \mainpage Heisprosjekt
 *
 * The documentation associated with the elevator project in TTK4235. 
 * 
 * \author Daniel Bjerkehagen & Simen Gangstad
 */

/**
 * @file 
 * 
 * @brief Main entry point for the elevator. Unit tests can be executed by passing 
 *        the @c --unit-test flag to the binary.
 */
#include <stdio.h>
#include <string.h>

#include "fsm.h"
#include "tests/unit_tests.h"

/**
 * @brief Main entry point of the elevator.
 * 
 * @param argc Argument count passed to the binary.
 * @param argv Argument values passed to the binary.
 * 
 * @return Exit status. 
 */
int main(const int argc, const char** argv) {
    if (argc == 2 && strcmp(argv[1], "-unit-test")) {
        unit_tests_check();
    } else {
        fsm_run();
    }

    return 0;
}