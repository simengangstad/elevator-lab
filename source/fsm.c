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
	int currentFloor = -1;

    while(1) {
		State nextState = fsmDecideNextState(currentState, priorityQueue, currentFloor);	

		if (nextState != currentState) {
			fsmTransition(currentState, nextState, priorityQueue);
			currentState = nextState;
			*shouldClearOrders = false;
		}

		for (unsigned int floor = 0; floor < HARDWARE_NUMBER_OF_FLOORS; floor++) {
			if (hardware_read_floor_sensor(floor)) {
				currentFloor = floor;
				break;
			}
		}

		fsmStateUpdate(currentState, shouldClearOrders);
		// doorUpdate();

		if (!shouldClearOrders && currentFloor != -1) {
			for (unsigned int floor = 0; floor < HARDWARE_NUMBER_OF_FLOORS; floor++) {
				for (HardwareOrder orderType = HARDWARE_ORDER_UP; orderType <= HARDWARE_ORDER_DOWN; orderType++) {

					if (hardware_read_order(floor, orderType)) {
						queueAddNode(nodeCreate(floor, orderType), priorityQueue, currentFloor);
						hardware_command_order_light(floor, orderType, true);
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

State fsmDecideNextState(State currentState, const Node* priorityQueue, const int currentFloor) {
	State nextState = currentState;

	switch (currentState) {
		case Startup:

			if (hardware_read_stop_signal()) {
				nextState = Stop;
			}
			else {
				for (unsigned int i = 0; i < HARDWARE_NUMBER_OF_FLOORS; i++) {
					if (hardware_read_floor_sensor(i)) {
						nextState = Idle;
					}
				}
			}

		break;

		case Idle:

			if (!hardware_read_stop_signal()) {
				if (queueIsEmpty(priorityQueue)) {
					nextState = Move;
				}
			}
			else {
				nextState = Stop;
			}

		break;

		case Move:

			if (hardware_read_stop_signal()) {
				nextState = Stop;
			}
			else {
				if (currentFloor == priorityQueue->floor) {
					nextState = DoorOpen;
				}
			}

		break;

		case DoorOpen:

			if (hardware_read_stop_signal()) {
				nextState = Stop;
			}
			else {
				/*
				if (doorIsOpen()) {
					if (!queueIsEmpty(priorityQueue)) {
						nextState = Move;	
					}
					else {
						nextState = Idle;
					}
				}*/
			}

		break;
		
		case Stop:
			if (!hardware_read_stop_signal()) {
				nextState = Startup;
			}
			
		break;

		default:
		break;
	}

	return nextState;
}

void fsmTransition(State currentState, State nextState, Node* priorityQueue) {

	// Perform exit for current state	
	switch (currentState) {
		case Startup:
			hardware_command_movement(HARDWARE_MOVEMENT_STOP);
		break;

		case Idle:
			// No exit operation
		break;

		case Move:
			hardware_command_movement(HARDWARE_MOVEMENT_STOP);
		break;

		case DoorOpen:
			// No exit
		break;
		
		case Stop:
			hardware_command_stop_light(false);
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
			hardware_command_movement(priorityQueue->direction);
		break;

		case DoorOpen:
			for (unsigned int orderType = HARDWARE_ORDER_UP; orderType <= HARDWARE_ORDER_DOWN; orderType++) {
				hardware_command_order_light(priorityQueue->floor, orderType, false);
			}

			hardware_command_floor_indicator_on(priorityQueue->floor);
			// TODO: Open and close door
			priorityQueue = queuePop(priorityQueue, priorityQueue->floor);

		break;
		
		case Stop:
			hardware_command_movement(HARDWARE_MOVEMENT_STOP);
			hardware_command_stop_light(true);
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
			// No update
		break;

		case Move:
			// No update
		break;

		case DoorOpen:
			// TODO: poll door
		break;
		
		case Stop:
		{

			for (unsigned int floor = 0; floor < HARDWARE_NUMBER_OF_FLOORS; floor++) {
				if (hardware_read_floor_sensor(floor)) {
					doorRequestOpenAndAutoclose();
				}
			}

			*shouldClearOrders = true;
		}
		break;

		default:
		break;
	}
	return;
}
