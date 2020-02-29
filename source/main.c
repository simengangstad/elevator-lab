/*! \mainpage Heisprosjekt
 *
 * The documentation associated with the elevator project in TTK4235. 
 * 
 * \author Daniel Bjerkehagen & Simen Gangstad
 */

#include <string.h>

#include "fsm.h"
#include "tests/unit_tests.h"

int main(const int argc, const char** argv) {
    if (argc == 2 && strcmp(argv[1], "--unit-test")) {
        unit_tests_check();
    } else {
        fsm_run();
    }

    return 0;
}