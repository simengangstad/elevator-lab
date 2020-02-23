/**
 * @brief Implementation of the Finite State Machine
 */

#include "fsm.h"

static void sigint_handler(int sig) {
    (void)(sig);
    printf("Terminating elevator\n");
    hardware_command_movement(HARDWARE_MOVEMENT_STOP);
    exit(0);
}

int main() {
    int error = hardware_init();
    if (error != 0) {
        fprintf(stderr, "Unable to initialize hardware\n");
        exit(1);
    }

    signal(SIGINT, sigint_handler);

    State currentState = Startup;
    Node *priorityQueue = NULL;

    while (1) {
        priorityQueue = queueAddNode(nodeCreate(0, HARDWARE_MOVEMENT_UP), priorityQueue, 3);
    }

    return 0;

    bool *shouldClearOrders = malloc(sizeof(bool));
    *shouldClearOrders = false;
    int currentFloor = -1;

    // Transition to the start up state so we enter it properly before the main loop, this will just ensure
    // that the code associated with the start up state is executed
    fsmTransition(Undefined, Startup, priorityQueue, currentFloor);

    while (1) {
        State nextState = fsmDecideNextState(currentState, priorityQueue, currentFloor);

        for (unsigned int floor = 0; floor < HARDWARE_NUMBER_OF_FLOORS; floor++) {
            if (hardware_read_floor_sensor(floor)) {
                currentFloor = floor;
                break;
            }
        }

        if (nextState != currentState) {
            fsmTransition(currentState, nextState, priorityQueue, currentFloor);
            currentState = nextState;
            *shouldClearOrders = false;
        }

        fsmStateUpdate(currentState, shouldClearOrders);
        doorUpdate();

        if (!*shouldClearOrders && currentFloor != -1) {
            for (unsigned int floor = 0; floor < HARDWARE_NUMBER_OF_FLOORS; floor++) {
                for (HardwareOrder orderType = HARDWARE_ORDER_UP; orderType <= HARDWARE_ORDER_DOWN; orderType++) {
                    if (hardware_read_order(floor, orderType)) {
                        priorityQueue = queueAddNode(nodeCreate(floor, orderType), priorityQueue, currentFloor);
                        hardware_command_order_light(floor, orderType, true);
                    }
                }
            }
        } else {
            queueClear(priorityQueue);
        }
    }

    free(shouldClearOrders);

    return 0;
}

State fsmDecideNextState(State currentState, const Node *priorityQueue, const int currentFloor) {
    State nextState = currentState;

    switch (currentState) {
        case Startup:

            if (hardware_read_stop_signal()) {
                nextState = Stop;
            } else {
                for (unsigned int i = 0; i < HARDWARE_NUMBER_OF_FLOORS; i++) {
                    if (hardware_read_floor_sensor(i)) {
                        nextState = Idle;
                    }
                }
            }

            break;

        case Idle:

            if (!hardware_read_stop_signal()) {
                if (!queueIsEmpty(priorityQueue)) {
                    nextState = Move;
                }
            } else {
                nextState = Stop;
            }

            break;

        case Move:

            if (hardware_read_stop_signal()) {
                nextState = Stop;
            } else {
                if (currentFloor == priorityQueue->floor) {
                    nextState = DoorOpen;
                }
            }

            break;

        case DoorOpen:

            if (hardware_read_stop_signal()) {
                nextState = Stop;
            } else {
                if (doorIsOpen()) {
                    if (!queueIsEmpty(priorityQueue)) {
                        nextState = Move;
                    } else {
                        nextState = Idle;
                    }
                }
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

void fsmTransition(State currentState, State nextState, Node *priorityQueue, const int currentFloor) {
    // Perform exit for current state
    switch (currentState) {
        case Startup:
            hardware_command_movement(HARDWARE_MOVEMENT_STOP);
            break;

        case Idle:
            // No exit operation
            break;

        case Move:
            // hardware_command_movement(HARDWARE_MOVEMENT_STOP);
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
        case Startup: {
            bool isAtFloor = false;

            for (unsigned int floor = 0; floor < HARDWARE_NUMBER_OF_FLOORS; floor++) {
                if (hardware_read_floor_sensor(floor)) {
                    isAtFloor = true;
                    hardware_command_floor_indicator_on(floor);
                }

                for (HardwareOrder orderType = HARDWARE_ORDER_UP; orderType <= HARDWARE_ORDER_DOWN; orderType++) {
                    hardware_command_order_light(floor, orderType, false);
                }
            }

            if (!isAtFloor) {
                hardware_command_movement(HARDWARE_MOVEMENT_DOWN);
            }
        } break;

        case Idle:
            // No enter
            break;

        case Move: {
            // TODO: what happens if we order to the current floor, where will it go?
            // HardwareMovement direction = priorityQueue->floor < currentFloor ? HARDWARE_MOVEMENT_DOWN : HARDWARE_MOVEMENT_UP;
            // hardware_command_movement(direction);
        } break;
        case DoorOpen:
            for (unsigned int orderType = HARDWARE_ORDER_UP; orderType <= HARDWARE_ORDER_DOWN; orderType++) {
                hardware_command_order_light(priorityQueue->floor, orderType, false);
            }

            hardware_command_floor_indicator_on(priorityQueue->floor);
            doorRequestOpenAndAutoclose();
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

void fsmStateUpdate(State currentState, bool *shouldClearOrders) {
    switch (currentState) {
        case Startup:
            *shouldClearOrders = true;
            break;

        case Idle:
            // No Update
            break;

        case Move:
            // No update
            break;

        case DoorOpen:
            // No update
            break;

        case Stop: {
            for (unsigned int floor = 0; floor < HARDWARE_NUMBER_OF_FLOORS; floor++) {
                if (hardware_read_floor_sensor(floor)) {
                    doorRequestOpenAndAutoclose();
                }
            }

            *shouldClearOrders = true;
        } break;

        default:
            break;
    }
    return;
}
