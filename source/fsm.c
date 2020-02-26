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

    HardwareMovement *p_current_movement = malloc(sizeof(HardwareMovement));
    *p_current_movement = HARDWARE_MOVEMENT_STOP;

    // Transition to the start up state so we enter it properly before the main loop, this will just ensure
    // that the code associated with the start up state is executed
    fsm_transition(UNDEFINED, STARTUP, &p_priority_queue, p_current_movement, current_floor);

    while (1) {
        State next_state = fsm_decide_next_state(current_state, p_priority_queue, current_floor);

        for (unsigned int floor = 0; floor < HARDWARE_NUMBER_OF_FLOORS; floor++) {
            if (hardware_read_floor_sensor(floor)) {
                current_floor = floor;
                break;
            }
        }

        if (next_state != current_state) {
            fsm_transition(current_state, next_state, &p_priority_queue, p_current_movement, current_floor);
            current_state = next_state;
            *p_should_clear_orders = false;
        }

        fsm_state_update(current_state, current_floor, p_should_clear_orders);
        door_update();

        if (!*p_should_clear_orders) {
            if (current_floor != -1) {
                for (unsigned int floor = 0; floor < HARDWARE_NUMBER_OF_FLOORS; floor++) {
                    for (HardwareOrder order_type = HARDWARE_ORDER_UP; order_type <= HARDWARE_ORDER_DOWN; order_type++) {
                        if (hardware_read_order(floor, order_type)) {
                            p_priority_queue = queue_add_node(node_create(floor, order_type), p_priority_queue, current_floor);
                            hardware_command_order_light(floor, order_type, true);
                        }
                    }
                }
            }
        } else {
            p_priority_queue = queue_clear(p_priority_queue);
        }
    }

    free(p_should_clear_orders);
    free(p_current_movement);

    return 0;
}

State fsm_decide_next_state(const State current_state, const Node *p_priority_queue, const int current_floor) {
    State next_state = current_state;

    switch (current_state) {
        case STARTUP: {
            if (hardware_read_stop_signal()) {
                next_state = STOP;
            } else {
                for (unsigned int floor = 0; floor < HARDWARE_NUMBER_OF_FLOORS; floor++) {
                    if (hardware_read_floor_sensor(floor)) {
                        next_state = IDLE;
                    }
                }
            }

        } break;

        case IDLE: {
            if (hardware_read_stop_signal()) {
                next_state = STOP;
            } else if (!queue_is_empty(p_priority_queue)) {
                next_state = MOVE;
            }

        } break;

        case MOVE: {
            if (hardware_read_stop_signal()) {
                next_state = STOP;
            } else if (current_floor == p_priority_queue->floor) {
                next_state = DOOR_OPEN;
            }

        } break;

        case DOOR_OPEN: {
            if (hardware_read_stop_signal()) {
                next_state = STOP;
            } else if (!door_is_open()) {
                if (queue_is_empty(p_priority_queue)) {
                    next_state = IDLE;
                } else {
                    next_state = MOVE;
                }
            }

        } break;

        case STOP: {
            if (!hardware_read_stop_signal()) {
                if (current_floor == -1) {  // TODO: Fiks denne jallamekken av case 2
                    next_state = STARTUP;
                } else {
                    next_state = IDLE;
                }
            }

        } break;

        default:
            break;
    }

    return next_state;
}

void fsm_transition(const State current_state, const State next_state, Node **pp_priority_queue, HardwareMovement *p_current_movement, const int current_floor) {
    // Perform exit for current state
    switch (current_state) {
        case STARTUP: {
            *p_current_movement = HARDWARE_MOVEMENT_STOP;
            hardware_command_movement(*p_current_movement);
        } break;

        case IDLE: {
            // No exit operation
        } break;

        case MOVE: {
            *p_current_movement = HARDWARE_MOVEMENT_STOP;
            hardware_command_movement(*p_current_movement);
        } break;

        case DOOR_OPEN: {
            // No exit
        } break;

        case STOP: {
            hardware_command_stop_light(false);

        } break;

        default:
            break;
    }

    // Perform enter for next state
    switch (next_state) {
        case STARTUP: {
            bool is_at_floor = false;

            for (unsigned int floor = 0; floor < HARDWARE_NUMBER_OF_FLOORS; floor++) {
                if (hardware_read_floor_sensor(floor)) {
                    is_at_floor = true;
                    hardware_command_floor_indicator_on(floor);
                }

                for (HardwareOrder order_type = HARDWARE_ORDER_UP; order_type <= HARDWARE_ORDER_DOWN; order_type++) {
                    hardware_command_order_light(floor, order_type, 0);
                }
            }

            if (!is_at_floor) {
                *p_current_movement = HARDWARE_MOVEMENT_DOWN;
                hardware_command_movement(*p_current_movement);
            }
        } break;

        case IDLE: {
            // No enter
        } break;

        case MOVE: {
            // TODO: what happens if we order to the current floor, where will it go? Add
            //       direction or bool "above"

            *p_current_movement = (*pp_priority_queue)->floor < current_floor ? HARDWARE_MOVEMENT_DOWN : HARDWARE_MOVEMENT_UP;
            hardware_command_movement(*p_current_movement);
        } break;

        case DOOR_OPEN: {
            for (unsigned int order_type = HARDWARE_ORDER_UP; order_type <= HARDWARE_ORDER_DOWN; order_type++) {
                hardware_command_order_light((*pp_priority_queue)->floor, order_type, false);
            }

            door_request_open_and_autoclose();
            *pp_priority_queue = queue_pop((*pp_priority_queue), (*pp_priority_queue)->floor);

        } break;

        case STOP: {
            hardware_command_movement(HARDWARE_MOVEMENT_STOP);
            hardware_command_stop_light(true);
            for (unsigned int floor = 0; floor < HARDWARE_NUMBER_OF_FLOORS; floor++) {
                for (unsigned int order_type = HARDWARE_ORDER_UP; order_type <= HARDWARE_ORDER_DOWN; order_type++) {
                    hardware_command_order_light(floor, order_type, false);
                }
            }
        } break;

        default:
            break;
    }
}

void fsm_state_update(const State current_state, const int current_floor, bool *p_should_clear_orders) {
    switch (current_state) {
        case STARTUP: {
            *p_should_clear_orders = true;
        } break;

        case IDLE: {
            // No Update
        } break;

        case MOVE: {
            hardware_command_floor_indicator_on(current_floor);
            // No update
        } break;

        case DOOR_OPEN: {
            // No update
        } break;

        case STOP: {
            for (unsigned int floor = 0; floor < HARDWARE_NUMBER_OF_FLOORS; floor++) {  // Nødvendig? Sender jo inn current_floor
                if (hardware_read_floor_sensor(floor)) {
                    door_request_open_and_autoclose();
                }
            }

            *p_should_clear_orders = true;
        } break;

        default:
            break;
    }
    return;
}
