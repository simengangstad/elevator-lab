#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <assert.h>
#include <unistd.h>

#include "hardware.h"
#include "../door.h"
#include "test_util.h"

static void sigint_handler(int sig){
    (void)(sig);
    printf("Terminating elevator\n");
    hardware_command_movement(HARDWARE_MOVEMENT_STOP);
    exit(0);
}


/**
 * @brief Will check if the door does not close when there is an obstruction. 
 *
 * @note Test TDOOR-1.
 *
 * @return True if the door did not close as there were an obstruction, false
 * 		   if not.
 */
static bool doorTestsCheckKeepsOpenWhileObstruction() {

	const float durationToCheck = 5.0f;
	const float sleepDuration = 0.5f;
	
	printf("Enable obstruction now please. Will open door and try to close for %f seconds. Press enter to continue...", durationToCheck);
	testUtilWaitUntilEnterKeyIsPressed();

	doorRequestOpenAndAutoclose();

	const clock_t startTime = clock();

	bool doorOpen = false;

	while ((float) (clock() - startTime) >= durationToCheck) {

		doorUpdate();

		doorOpen = doorIsOpen();

		if (!doorOpen) {
			break;
		}

		sleep(sleepDuration);
	}

	return doorOpen;
}
	
void doorTestsValidate() {

	printf("Starting door tests.\n");
	printf("1. Check if door keeps open when there is an obstruction");
	assert(doorTestsCheckKeepsOpenWhileObstruction());
	printf("1. Passed");
}
