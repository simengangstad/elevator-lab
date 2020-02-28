/**
 * @brief Implementation of the Finite State Machine
 */

#include "fsm.h"

static void sigint_handler(int sig);

const char* get_state_name(const State state);

const char* get_movement_name(const HardwareMovement movement);

// TODO: IS this at bit to unreadable?

/**
 * @brief Checks if the elevator is currently at a floor.
 * 
 * @param [out] current_floor Will be overwritten with the floor number if the elevator is at a given floor.
 * 
 * @return True of the elevator is at a floor.
 */
static bool fsm_elevator_is_at_floor(int* current_floor);

/**
 * @brief Checks where the elevator is given @p movement.
 * 
 * @param [in] last_floor The floor the elevator was last recorded to be at.
 * @param [in] movement The current movement of the elevator.
 * 
 * @return The position of the elevator.
 */
static Position fsm_decide_position(const int last_floor, const HardwareMovement movement);

/**
 * @brief Polls the current orders and puts them in the @p p_priority_queue.
 * 
 * @param [in] p_priority_queue The current queue.
 * @param [in] current_position The position of the elevator, used in the queue algorithm to decide where the new orders
 *                              should be placed.
 * 
 * @return The updated queue.
 */
static Node* fsm_poll_orders_and_update_queue(Node* p_priority_queue, const Position current_position);

int main() {
    int error = hardware_init();
    if (error != 0) {
        fprintf(stderr, "Unable to initialize hardware\n");
        exit(1);
    }

    signal(SIGINT, sigint_handler);

    State current_state = STATE_UNDEFINED;
    int last_floor = FLOOR_UNDEFINED;
    Position current_position = {FLOOR_UNDEFINED, OFFSET_UNDEFINED};
    Node* p_priority_queue = NULL;
    HardwareMovement* p_last_movement = malloc(sizeof(HardwareMovement));
    *p_last_movement = HARDWARE_MOVEMENT_STOP;

    while (1) {
        current_position = fsm_decide_position(last_floor, *p_last_movement);

        if (current_position.floor != FLOOR_UNDEFINED && current_position.offset == OFFSET_AT_FLOOR) {
            hardware_command_floor_indicator_on(current_position.floor);
            last_floor = current_position.floor;
        }

        State next_state = fsm_decide_next_state(current_state, p_priority_queue, current_position);

        if (next_state != current_state) {
            fsm_transition(current_state, next_state, &p_priority_queue, p_last_movement, current_position);
            current_state = next_state;
        }

        fsm_state_update(current_state, &p_priority_queue, current_position);
        door_update();

        printf("CURRENT STATE: %s\n", get_state_name(current_state));
        printf("CURRENT FLOOR: %d\n", current_position.floor);
        printf("LAST MOVEMENT: %s\n", get_movement_name(*p_last_movement));
        queue_print(p_priority_queue);
        printf("=================\n");
    }

    free(p_last_movement);
    queue_clear(p_priority_queue);

    return 0;
}

static void sigint_handler(int sig) {
    (void)(sig);
    printf("Terminating elevator\n");
    hardware_command_movement(HARDWARE_MOVEMENT_STOP);
    exit(0);
}

const char* get_state_name(const State state) {
    switch (state) {
        case STATE_STARTUP:
            return "Startup";
        case STATE_IDLE:
            return "Idle";
        case STATE_MOVE:
            return "Move";
        case STATE_DOOR_OPEN:
            return "DoorOpen";
        case STATE_STOP:
            return "Stop";
        case STATE_UNDEFINED:
            return "Undefined";
        default:
            return "Not a state";
    }
}

const char* get_movement_name(const HardwareMovement movement) {
    switch (movement) {
        case HARDWARE_MOVEMENT_UP:
            return "Up";
        case HARDWARE_MOVEMENT_STOP:
            return "Stop";
        case HARDWARE_MOVEMENT_DOWN:
            return "Down";
        default:
            return "Not a movement";
    }
}

static bool fsm_elevator_is_at_floor(int* current_floor) {
    for (unsigned int floor = 0; floor < HARDWARE_NUMBER_OF_FLOORS; floor++) {
        if (hardware_read_floor_sensor(floor)) {
            *current_floor = floor;
            return true;
        }
    }

    *current_floor = FLOOR_UNDEFINED;

    return false;
}

static Position fsm_decide_position(const int last_floor, const HardwareMovement movement) {
    int new_floor = last_floor;
    Offset offset = OFFSET_UNDEFINED;

    int current_floor = last_floor;
    if (fsm_elevator_is_at_floor(&current_floor)) {
        new_floor = current_floor;
        offset = OFFSET_AT_FLOOR;
    }

    if (offset != OFFSET_AT_FLOOR && movement == HARDWARE_MOVEMENT_DOWN) {
        offset = OFFSET_BELOW;
    } else if (offset != OFFSET_AT_FLOOR && movement == HARDWARE_MOVEMENT_UP) {
        offset = OFFSET_ABOVE;
    }

    return (Position){new_floor, offset};
}

static Node* fsm_poll_orders_and_update_queue(Node* p_priority_queue, const Position current_position) {
    for (unsigned int floor = 0; floor < HARDWARE_NUMBER_OF_FLOORS; floor++) {
        for (HardwareOrder order_type = HARDWARE_ORDER_UP; order_type <= HARDWARE_ORDER_DOWN; order_type++) {
            if (hardware_read_order(floor, order_type)) {
                p_priority_queue = queue_add_node(node_create(floor, order_type), p_priority_queue, current_position.floor);
                hardware_command_order_light(floor, order_type, true);
            }
        }
    }

    return p_priority_queue;
}

State fsm_decide_next_state(const State current_state, const Node* p_priority_queue, const Position current_position) {
    State next_state = current_state;

    switch (current_state) {
        case STATE_UNDEFINED:
            next_state = STATE_STARTUP;
            break;

        case STATE_STARTUP: {
            if (hardware_read_stop_signal()) {
                next_state = STATE_STOP;
            } else if (current_position.offset == OFFSET_AT_FLOOR) {
                next_state = STATE_IDLE;
            }

        } break;

        case STATE_IDLE: {
            if (hardware_read_stop_signal()) {
                next_state = STATE_STOP;
            } else if (!queue_is_empty(p_priority_queue)) {
                next_state = STATE_MOVE;
            }

        } break;

        case STATE_MOVE: {
            if (hardware_read_stop_signal()) {
                next_state = STATE_STOP;
            } else if (current_position.floor == p_priority_queue->floor && current_position.offset == OFFSET_AT_FLOOR) {
                next_state = STATE_DOOR_OPEN;
            }

        } break;

        case STATE_DOOR_OPEN: {
            if (hardware_read_stop_signal()) {
                next_state = STATE_STOP;
            } else if (!door_is_open()) {
                if (queue_is_empty(p_priority_queue)) {
                    next_state = STATE_IDLE;
                } else {
                    next_state = STATE_MOVE;
                }
            }

        } break;

        case STATE_STOP: {
            if (!hardware_read_stop_signal()) {
                if (door_is_open()) {
                    next_state = STATE_DOOR_OPEN;       // Potensielt problem: door_open sine enter-greier
                }
                else {
                    next_state = STATE_IDLE;
                }
            }

        } break;

        default:
            break;
    }

    return next_state;
}

void fsm_transition(const State current_state, const State next_state, Node** pp_priority_queue, HardwareMovement* p_last_movement, const Position current_position) {
    // Perform exit for current state
    switch (current_state) {
        case STATE_STARTUP: {
            *p_last_movement = HARDWARE_MOVEMENT_STOP;
            hardware_command_movement(*p_last_movement);
        } break;

        case STATE_IDLE: {
            // No exit
        } break;

        case STATE_MOVE: {
            hardware_command_movement(HARDWARE_MOVEMENT_STOP);
        } break;

        case STATE_DOOR_OPEN: {
            // No exit
        } break;

        case STATE_STOP: {
            hardware_command_stop_light(false);

        } break;

        default:
            break;
    }

    // Perform enter for next state
    switch (next_state) {
        case STATE_STARTUP: {
            bool is_at_floor = false;

            for (unsigned int floor = 0; floor < HARDWARE_NUMBER_OF_FLOORS; floor++) {
                if (hardware_read_floor_sensor(floor)) {  // Her itererer vi likevel over alle etasjene, hvorfor ikke sette current_floor her? -> Sentralisering av setting av globale var.
                    is_at_floor = true;
                }

                // Clear all existing order lights
                for (HardwareOrder order_type = HARDWARE_ORDER_UP; order_type <= HARDWARE_ORDER_DOWN; order_type++) {  // Hvorfor er dette med?
                    hardware_command_order_light(floor, order_type, 0);
                }
            }

            if (!is_at_floor) {
                *p_last_movement = HARDWARE_MOVEMENT_DOWN;
                hardware_command_movement(*p_last_movement);
            }
        } break;

        case STATE_IDLE: {
            // No enter
        } break;

        case STATE_MOVE: {
            HardwareMovement new_movement = *p_last_movement;
            new_movement = (*pp_priority_queue)->floor < current_position.floor ? HARDWARE_MOVEMENT_DOWN : HARDWARE_MOVEMENT_UP;

            // If we stopped between the floors and order to the current floor we decide direction based
            // on the previous direction
            if (current_position.offset == OFFSET_BELOW && (*pp_priority_queue)->floor == current_position.floor) {
                new_movement = HARDWARE_MOVEMENT_UP;
            } else if (current_position.offset == OFFSET_ABOVE && (*pp_priority_queue)->floor == current_position.floor) {
                new_movement = HARDWARE_MOVEMENT_DOWN;
            }

            hardware_command_movement(new_movement);
            *p_last_movement = new_movement;

        } break;

        case STATE_DOOR_OPEN: {
            if (!queue_is_empty(*pp_priority_queue)) {
                for (unsigned int order_type = HARDWARE_ORDER_UP; order_type <= HARDWARE_ORDER_DOWN; order_type++) {
                    hardware_command_order_light((*pp_priority_queue)->floor, order_type, false);
                }
                *pp_priority_queue = queue_pop((*pp_priority_queue), (*pp_priority_queue)->floor);
            }
            door_request_open_and_autoclose();
            

        } break;

        case STATE_STOP: {
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

void fsm_state_update(const State current_state, Node** pp_priority_queue, const Position current_position) {
    switch (current_state) {
        case STATE_STARTUP: {
            *pp_priority_queue = queue_clear(*pp_priority_queue);
        } break;

        case STATE_IDLE: {
            *pp_priority_queue = fsm_poll_orders_and_update_queue(*pp_priority_queue, current_position);
        } break;

        case STATE_MOVE: {
            *pp_priority_queue = fsm_poll_orders_and_update_queue(*pp_priority_queue, current_position);
        } break;

        case STATE_DOOR_OPEN: {
            *pp_priority_queue = fsm_poll_orders_and_update_queue(*pp_priority_queue, current_position);
        } break;

        case STATE_STOP: {
            for (unsigned int floor = 0; floor < HARDWARE_NUMBER_OF_FLOORS; floor++) {
                if (hardware_read_floor_sensor(floor)) {
                    door_request_open_and_autoclose();
                }
            }

            *pp_priority_queue = queue_clear(*pp_priority_queue);
        } break;

        default:
            break;
    }
    return;
}
