/**
 * @file
 * @brief Implementation of tests for the door module.
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <assert.h>
#include <unistd.h>

#include "hardware.h"
#include "../door.h"
#include "test_util.h"

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

/**
 * @brief Will check if the door closes when there is NOT an obstruction. This function will also implicitely check if
 * the timer for the door gets setup correctly, updates and that the door closes after 3 seconds.
 *
 * @note Test TDOOR-2, TDOOR-3, TDOOR-4, TDOOR-6
 *
 * @return True if the door did close, false if not.
 */
static bool doorTestsCheckClosesWhenNoObstruction() {

	const float durationToCheck = 3.5f;
	const float sleepDuration = 0.1f;
	
	printf("Disable obstruction now please. Will open door and try to autoclose. Press enter to continue...");
	testUtilWaitUntilEnterKeyIsPressed();

	doorRequestOpenAndAutoclose();

	const clock_t startTime = clock();

	while ((float) (clock() - startTime) >= durationToCheck) {

		doorUpdate();
		sleep(sleepDuration);
	}

	return doorIsOpen();
}


/**
 * @brief Checks if the timer gets reset when then the obstruction signal goes high.
 *
 * @note Test TDOOR-5
 */
static void doorTestsCheckTimerReset() {

	const float sleepDuration = 0.1f;
	
	printf("Enable obstruction now please. Will open door and try to autoclose. Disable obstruction signal when ready and count the seconds it takes for the door to close. Press enter to continue...");
	testUtilWaitUntilEnterKeyIsPressed();
	doorRequestOpenAndAutoclose();

	while (doorIsOpen()) {
		doorUpdate();
		sleep(sleepDuration);
	}
}


/**
 * @brief Checks if the the door module returns the right value for when the door is open. 
 *
 * @note Test TDOOR-7 
 *
 * @return True if the function for checking if the door is open returned the right value, false if not.
 */
static bool doorTestsCheckDoorIsOpenFunction() {

	const float durationToCheck = 3.5f;
	const float sleepDuration = 0.1f;
	
	printf("Disable obstruction please. Will open door and try to autoclose. Press enter to continue...");
	testUtilWaitUntilEnterKeyIsPressed();

	doorRequestOpenAndAutoclose();
	const bool doorOpened = doorIsOpen();

	const clock_t startTime = clock();

	while ((float) (clock() - startTime) >= durationToCheck) {

		doorUpdate();
		sleep(sleepDuration);
	}

	const bool doorClosed = !doorIsOpen();

	return doorOpened && doorClosed;
}


void doorTestsValidate() {

	printf("Starting door tests.\n");

	printf("1. Check if door keeps open when there is an obstruction (TDOOR-1)");
	assert(doorTestsCheckKeepsOpenWhileObstruction());
	printf("1. Passed");

	printf("2. Check if door closes when there is no obstruction (TDOOR-2, TDOOR-3, TDOOR-4, TDOOR-6)");
	assert(doorTestsCheckClosesWhenNoObstruction());
	printf("2. Passed");

	printf("3. Check if the timer resets. Validate visually (TDOOR-5)");
	doorTestsCheckTimerReset();
	printf("3. End");

	printf("4. Check if the function for whether the door is open returns the right value (TDOOR-7).");
	assert(doorTestsCheckDoorIsOpenFunction());
	printf("4. Passed");
}
