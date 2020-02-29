/**
 * @file 
 * 
 * @brief Entry point for the unit tests.
 */

#ifndef UNIT_TESTS_H
#define UNIT_TESTS_H

#include <signal.h>

#include "door_tests.h"
#include "fsm_tests.h"
#include "priority_queue_tests.h"

/**
 * @brief Checks all the unit tests,
 */
void unit_tests_check();

#endif