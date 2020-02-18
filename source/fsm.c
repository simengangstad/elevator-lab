/**
 * @brief Implementation of the Finite State Machine
 */

#include <stdbool.h>

void fsmUpdate(State currentState) {

	State nextState = fsmDecideNextState(currentState);


	return;
}

void fsmUpdateLights() {
	// Kode her
	return;
}

State fsmDecideNextState(State currentState) {
	State nextState;

	switch (currentState) {
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
	// Kode her
	return;
}

void fsmStateEnter(State currentState) {
	switch (currentState) {
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

void fsmStateExit(State currentState) {
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
	return;
}

void fsmStateUpdate(State currentState) {
	// Kode her
	return;
}
