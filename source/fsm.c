/**
 * @brief Implementation of the Finite State Machine
 */

#include "fsm.h"

int main() {

    int error = hardware_init();
    if(error != 0){
        fprintf(stderr, "Unable to initialize hardware\n");
        exit(1);
    }


	// Transition to the start up state so we enter it properly before the main loop, this will just ensure
	// that the code associated with the start up state is executed
	// fsmTransition(Undefined, Startup);

	State currentState = Startup;
	Node* priorityQueue = NULL;

	bool* shouldClearOrders = malloc(sizeof(bool));
	
    while(1) {
		State nextState = fsmDecideNextState(currentState, priorityQueue);	

		if (nextState != currentState) {
			fsmTransition(currentState, nextState);
			currentState = nextState;
			*shouldClearOrders = false;
		}

		fsmStateUpdate(currentState, shouldClearOrders);

		if (!shouldClearOrders) {
			for (unsigned int floor = 0; floor < HARDWARE_NUMBER_OF_FLOORS; floor++) {
				for (HardwareOrder orderType = HARDWARE_ORDER_UP; orderType <= HARDWARE_ORDER_DOWN; orderType++) {

					if (hardware_read_order(floor, orderType)) {
						// queueAddNode(nodeCreate(floor, orderType), priority_queue, );
					}
				}
			}
		}
		else {
			queueClear(priorityQueue);
		}
    }

	free(shouldClearOrders);

	return 0;
}

void fsmUpdateLights() {
	// Kode her
	return;
}

State fsmDecideNextState(State currentState, const Node* priorityQueue) {
	State nextState = currentState;

	switch (currentState) {
		case Startup:

			for (unsigned int i = 0; i < HARDWARE_NUMBER_OF_FLOORS; i++) {
				if (hardware_read_floor_sensor(i)) {
					nextState = Idle;
				}
			}

		break;

		case Idle:

			if (!hardware_read_stop_signal() && !priorityQueue) {
				nextState = Move;
			}


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
			{
				bool isAtFloor = true;

				for (unsigned int floor = 0; floor < HARDWARE_NUMBER_OF_FLOORS; floor++) {
					isAtFloor = hardware_read_floor_sensor(floor);
				}

				if (!isAtFloor) {
					hardware_command_movement(HARDWARE_MOVEMENT_DOWN);
				}	
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

void fsmStateUpdate(State currentState, bool* shouldClearOrders) {

	switch (currentState) {
		case Startup:
			*shouldClearOrders = true;
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
