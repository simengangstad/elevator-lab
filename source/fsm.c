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

const char *get_state_name(const State state) {
    switch (state) {
        case STARTUP:
            return "Startup";
        case IDLE:
            return "Idle";
        case MOVE:
            return "Move";
        case DOOR_OPEN:
            return "DoorOpen";
        case STOP:
            return "Stop";
        case UNDEFINED:
            return "Undefined";
        default:
            return "Not a state";
    }
}

int main() {
    int error = hardware_init();
    if (error != 0) {
        fprintf(stderr, "Unable to initialize hardware\n");
        exit(1);
    }

    signal(SIGINT, sigint_handler);

    State current_state = STARTUP;
    Node *p_priority_queue = NULL;

    bool *p_should_clear_orders = malloc(sizeof(bool));
    *p_should_clear_orders = false;
    int current_floor = -1;

    // Transition to the start up state so we enter it properly before the main loop, this will just ensure
    // that the code associated with the start up state is executed
    fsmTransition(UNDEFINED, STARTUP, &p_priority_queue, current_floor);

    while (1) {
        State nextState = fsmDecideNextState(currentState, priorityQueue, currentFloor);

        for (unsigned int floor = 0; floor < HARDWARE_NUMBER_OF_FLOORS; floor++) {
            if (hardware_read_floor_sensor(floor)) {
                currentFloor = floor;
                break;
            }
        }

        if (nextState != currentState) {
            fsmTransition(currentState, nextState, &priorityQueue, currentFloor);
            currentState = nextState;
            *shouldClearOrders = false;
        }

        fsmStateUpdate(currentState, shouldClearOrders);
        doorUpdate();

        if (!*shouldClearOrders) {
            if (currentFloor != -1) {
                for (unsigned int floor = 0; floor < HARDWARE_NUMBER_OF_FLOORS; floor++) {
                    for (HardwareOrder orderType = HARDWARE_ORDER_UP; orderType <= HARDWARE_ORDER_DOWN; orderType++) {
                        if (hardware_read_order(floor, orderType)) {
                            priorityQueue = queueAddNode(nodeCreate(floor, orderType), priorityQueue, currentFloor);
                            hardware_command_order_light(floor, orderType, true);
                        }
                    }
                }
            }
        } else {
            priorityQueue = queueClear(priorityQueue);
        }
    }

    free(shouldClearOrders);

    return 0;
}

State fsm_decide_next_state(const State current_state, const Node *p_priority_queue, const int current_floor) {
    State next_state = current_state;

    switch (current_state) {
        case STARTUP:

            if (hardware_read_stop_signal()) {
                next_state = STOP;
            } else {
                for (unsigned int floor = 0; floor < HARDWARE_NUMBER_OF_FLOORS; floor++) {
                    if (hardware_read_floor_sensor(floor)) {
                        next_state = IDLE;
                    }
                }
            }

            break;

        case IDLE:

            if (hardware_read_stop_signal()) {
                next_state = STOP;
            } else if (!queue_is_empty(p_priority_queue)) {
                next_state = MOVE;
            }

            break;

        case MOVE:

            if (hardware_read_stop_signal()) {
                next_state = STOP;
            } else if (current_floor == p_priority_queue->floor) {
                next_state = DOOR_OPEN;
            }

            break;

        case DOOR_OPEN:

            if (hardware_read_stop_signal()) {
                next_state = STOP;
            } else if (!door_is_open()) {
                if (queue_is_empty(p_priority_queue)) {
                    next_state = IDLE;
                } else {
                    next_state = MOVE;
                }
            }

            break;

        case STOP:
            if (!hardware_read_stop_signal()) {
                next_state = IDLE;
            }

            break;

        default:
            break;
    }

    return next_state;
}

void fsmTransition(State currentState, State nextState, Node **priorityQueuePtr, const int currentFloor) {
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
        case Startup: {
            bool isAtFloor = false;

            for (unsigned int floor = 0; floor < HARDWARE_NUMBER_OF_FLOORS; floor++) {
                if (hardware_read_floor_sensor(floor)) {
                    isAtFloor = true;
                    hardware_command_floor_indicator_on(floor);
                }

                for (HardwareOrder orderType = HARDWARE_ORDER_UP; orderType <= HARDWARE_ORDER_DOWN; orderType++) {
                    hardware_command_order_light(floor, orderType, 0);
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
            // TODO: what happens if we order to the current floor, where will it go? Add
            //       direction
            HardwareMovement direction = (*priorityQueuePtr)->floor < currentFloor ? HARDWARE_MOVEMENT_DOWN : HARDWARE_MOVEMENT_UP;
            hardware_command_movement(direction);
        } break;
        case DoorOpen:
            for (unsigned int orderType = HARDWARE_ORDER_UP; orderType <= HARDWARE_ORDER_DOWN; orderType++) {
                hardware_command_order_light((*priorityQueuePtr)->floor, orderType, false);
            }

            hardware_command_floor_indicator_on((*priorityQueuePtr)->floor);
            doorRequestOpenAndAutoclose();
            *priorityQueuePtr = queuePop((*priorityQueuePtr), (*priorityQueuePtr)->floor);

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
