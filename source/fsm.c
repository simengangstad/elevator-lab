/**
 * @brief Implementation of the Finite State Machine
 */

#include <stdbool.h>
#include <stdio.h>

#include "fsm.h"

int main() {

    int error = hardware_init();
    if(error != 0){
        fprintf(stderr, "Unable to initialize hardware\n");
        exit(1);
    }

	State currentState = Startup;

	// Transition to the start up state so we enter it properly before the main loop
	fsmTransition(Undefined, currentState);

    while(1) {
		State previousState = currentState;
		State nextState = fsmDecideNextState(currentState);	

		if (nextState != previousState) {
			fsmTransition(currentState, nextState);
		}

		for (unsigned int i = 0; i < HARDWARE_NUMBER_OF_FLOORS; i++) {

			if (hardware_read_order(i, HARDWARE_ORDER_UP)) {
				
			}

			if (hardware_read_order(i, HARDWARE_ORDER_INSIDE)) {

			}

			if (hardware_read_order(i, HARDWARE_ORDER_DOWN)) {

			}
		}
    }

	return 0;
}

void fsmUpdateLights() {
	// Kode her
	return;
}

State fsmDecideNextState(State currentState) {
	State nextState;

	switch (currentState) {
		case Startup:

			for (unsigned int i = 0; i < HARDWARE_NUMBER_OF_FLOORS; i++) {
				if (hardware_read_floor_sensor(i)) {
					nextState = Idle;
				}
			}

		break;

		case Idle:

		break;

		case Move:

		break;

		case DoorOpen:

		break;
		
		case Stop:

		break;

		default:
		break;
	}

	return nextState;
}

void fsmTransition(State currentState, State nextState) {

	// Perform exit for current state	
	switch (currentState) {
		case Startup:
			hardware_command_movement(HARDWARE_MOVEMENT_STOP);
		break;

		case Idle:

		break;

		case Move:

		break;

		case DoorOpen:

		break;
		
		case Stop:

		break;

		default:
		break;
	}

	// Perform enter for next state	
	switch (nextState) {
		case Startup:
			bool isAtFloor = true;

			for (unsigned int i = 0; i < HARDWARE_NUMBER_OF_FLOORS; i++) {
				isAtFloor = hardware_read_floor_sensor(i);
			}

			if (!isAtFloor) {
				hardware_command_movement(HARDWARE_MOVEMENT_DOWN);
			}	

		break;

		case Idle:
			// No enter
		break;

		case Move:

		break;

		case DoorOpen:

		break;
		
		case Stop:

		break;

		default:
		break;
	}
}

void fsmStateUpdate(State currentState) {

	switch (currentState) {
		case Startup:
			// Ignore enqueuements
		break;

		case Idle:
			
		break;

		case Move:

		break;

		case DoorOpen:

		break;
		
		case Stop:

		break;

		default:
		break;
	}
	return;
}
