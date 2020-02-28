/**
 * @file
 * 
 * @brief Tests for the door module.
 */

#ifndef DOOR_TESTS_H
#define DOOR_TESTS_H

#include <assert.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#include "../door.h"
#include "../hardware.h"
#include "test_util.h"

/**
 * @brief Goes through all the tests specified for the door.
 */
void door_tests_validate();

#endif
