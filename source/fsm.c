/**
 * @file
 * @brief Implementation of the finite state machine
 */

#include "fsm.h"

#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "door.h"

/**
 * @brief Gets the floor the elevator is currently at.
 * 
 * @return The floor number if the elevator is at a floor, #FLOOR_UNDEFINED if else.
 */
static int fsm_get_current_floor();

/**
 * @brief Checks if the elevator is at any floor based on the @p position. 
 * 
 * @param [in] position The position of the elevator.
 * 
 * @return true if the elevator is at any floor.
 */
static bool fsm_elevator_is_at_a_floor(const Position position);

/**
 * @brief Checks where the elevator is given the @p last_floor and @p movement_when_left_floor.
 * 
 * @param [in] last_floor The floor the elevator was last recorded to be at.
 * @param [in] movement_when_left_floor The movement when the elevator last left a floor. 
 * 
 * @return The position of the elevator.
 */
static Position fsm_decide_elevator_position(const int last_floor, const HardwareMovement movement_when_left_floor);

/**
 * @brief Clears all the order lights.
 */
static void fsm_clear_order_lights();

/**
 * @brief Polls the current orders and puts them in the @p pp_priority_queue. Updates the order light for the new 
 *        order(s).
 * 
 * @param [in, out] pp_priority_queue The current queue.
 * @param [in] current_position The position of the elevator, used in the queue algorithm to decide where the new 
 *             orders should be placed.
 */
static void fsm_manage_orders_and_update_queue(Order** pp_priority_queue, const Position current_position);

/**
 * @brief Checks if the top order in the @p p_priority_queue is at the @p floor.
 * 
 * @param [in] p_priority_queue The priority queue to check. 
 * @param [in] floor The floor to check the top order against. 
 * 
 * @return true If the top order is at the @p floor. 
 */
static bool fsm_top_order_is_at_current_floor(const Order* p_priority_queue, const int floor);

/**
 * @brief Clears the top order of the @p pp_priority_queue and turns off the lights for the floor the top
 *        order is pointing to.
 * 
 * @param [in, out] pp_priority_queue The priority queue to clear the top order from. 
 * 
 */
static void fsm_clear_top_order_and_update_order_lights(Order** pp_priority_queue);

/**
 * @brief Handles signal interrupt from the command line.
 * 
 * @param sig The signal.
 */
static void fsm_sigint_handler(int sig);

/**
 * @brief Determines if the FSM should continue running.
 */
static bool m_fsm_should_abort = false;

/**
 * #################################################################################################################
 * #####                                       FSM LOGIC                                                       #####
 * #################################################################################################################
 */

void fsm_run() {
    int error = hardware_init();
    if (error != 0) {
        fprintf(stderr, "Unable to initialize hardware\n");
        exit(1);
    }

    signal(SIGINT, fsm_sigint_handler);

    State current_state = STATE_UNDEFINED;

    int last_floor = FLOOR_UNDEFINED;
    Position current_position = {FLOOR_UNDEFINED, OFFSET_UNDEFINED};

    Order* p_priority_queue = NULL;

    HardwareMovement* p_movement_when_left_floor = malloc(sizeof(HardwareMovement));
    *p_movement_when_left_floor = HARDWARE_MOVEMENT_STOP;

    while (!m_fsm_should_abort) {
        current_position = fsm_decide_elevator_position(last_floor, *p_movement_when_left_floor);

        if (fsm_elevator_is_at_a_floor(current_position)) {
            hardware_command_floor_indicator_on(current_position.floor);
            last_floor = current_position.floor;
        }

        State next_state = fsm_decide_next_state(current_state, p_priority_queue, current_position);

        if (next_state != current_state) {
            fsm_transition(current_state, next_state, &p_priority_queue, p_movement_when_left_floor, current_position);
            current_state = next_state;
        }

        fsm_state_update(current_state, &p_priority_queue, current_position);
        door_update();
        priority_queue_print(p_priority_queue);
    }

    printf("Terminating elevator\n");
    free(p_movement_when_left_floor);
    hardware_command_movement(HARDWARE_MOVEMENT_STOP);
}

State fsm_decide_next_state(const State current_state, const Order* p_priority_queue, const Position current_position) {
    State next_state = current_state;

    switch (current_state) {
        case STATE_UNDEFINED:
            next_state = STATE_STARTUP;
            break;
        case STATE_STARTUP: {
            if (hardware_read_stop_signal()) {
                next_state = STATE_STOP;
            } else if (fsm_elevator_is_at_a_floor(current_position)) {
                next_state = STATE_IDLE;
            }
        } break;

        case STATE_IDLE: {
            if (hardware_read_stop_signal()) {
                next_state = STATE_STOP;
            } else if (!priority_queue_is_empty(p_priority_queue)) {
                next_state = STATE_MOVE;
            }
        } break;

        case STATE_MOVE: {
            if (hardware_read_stop_signal()) {
                next_state = STATE_STOP;
            } else if (p_priority_queue->floor == current_position.floor && current_position.offset == OFFSET_AT_FLOOR) {
                next_state = STATE_DOOR_OPEN;
            }
        } break;

        case STATE_DOOR_OPEN: {
            if (hardware_read_stop_signal()) {
                next_state = STATE_STOP;
            } else if (!door_is_open() && priority_queue_is_empty(p_priority_queue)) {
                next_state = STATE_IDLE;
            } else if (!door_is_open() && !priority_queue_is_empty(p_priority_queue)) {
                next_state = STATE_MOVE;
            }
        } break;

        case STATE_STOP: {
            if (!hardware_read_stop_signal()) {
                if (door_is_open()) {
                    next_state = STATE_DOOR_OPEN;
                } else if (!door_is_open() && current_position.floor == FLOOR_UNDEFINED) {
                    next_state = STATE_STARTUP;
                } else if (!door_is_open() && current_position.floor != FLOOR_UNDEFINED) {
                    next_state = STATE_IDLE;
                }
            }
        } break;
    }

    return next_state;
}

void fsm_transition(const State current_state,
                    const State next_state,
                    Order** pp_priority_queue,
                    HardwareMovement* p_movement_when_left_floor,
                    const Position current_position) {
    // Perform exit for current state
    switch (current_state) {
        case STATE_STARTUP: {
            hardware_command_movement(HARDWARE_MOVEMENT_STOP);
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
            fsm_clear_order_lights();

            if (!fsm_elevator_is_at_a_floor(current_position)) {
                hardware_command_movement(HARDWARE_MOVEMENT_DOWN);
            }
        } break;

        case STATE_IDLE: {
            // No enter
        } break;

        case STATE_MOVE: {
            HardwareMovement new_movement = (*pp_priority_queue)->floor < current_position.floor ? HARDWARE_MOVEMENT_DOWN : HARDWARE_MOVEMENT_UP;

            // If we stopped between floors and order to the current floor we decide movement based
            // on the elevators offset to the current floor
            if ((*pp_priority_queue)->floor == current_position.floor && current_position.offset == OFFSET_BELOW) {
                new_movement = HARDWARE_MOVEMENT_UP;
            } else if ((*pp_priority_queue)->floor == current_position.floor && current_position.offset == OFFSET_ABOVE) {
                new_movement = HARDWARE_MOVEMENT_DOWN;
            }

            hardware_command_movement(new_movement);

            // Only update the movement when the elevator is at a floor and leaving
            if (fsm_elevator_is_at_a_floor(current_position)) {
                *p_movement_when_left_floor = new_movement;
            }
        } break;

        case STATE_DOOR_OPEN: {
            // No enter
        } break;

        case STATE_STOP: {
            hardware_command_movement(HARDWARE_MOVEMENT_STOP);
            hardware_command_stop_light(true);
            fsm_clear_order_lights();
            *pp_priority_queue = priority_queue_clear(*pp_priority_queue);
        } break;

        default:
            break;
    }
}

void fsm_state_update(const State current_state, Order** pp_priority_queue, const Position current_position) {
    switch (current_state) {
        case STATE_STARTUP: {
            // No update
        } break;

        case STATE_IDLE: {
            fsm_manage_orders_and_update_queue(pp_priority_queue, current_position);
        } break;

        case STATE_MOVE: {
            fsm_manage_orders_and_update_queue(pp_priority_queue, current_position);
        } break;

        case STATE_DOOR_OPEN: {
            fsm_manage_orders_and_update_queue(pp_priority_queue, current_position);

            if (fsm_top_order_is_at_current_floor(*pp_priority_queue, current_position.floor)) {
                fsm_clear_top_order_and_update_order_lights(pp_priority_queue);
                door_request_open_and_autoclose();
            }

        } break;

        case STATE_STOP: {
            if (fsm_elevator_is_at_a_floor(current_position)) {
                door_request_open_and_autoclose();
            }
        } break;

        default:
            break;
    }
    return;
}

/**
 * #################################################################################################################
 * #####                                       FLOOR & POSITION LOGIC                                          #####
 * #################################################################################################################
 */

static int fsm_get_current_floor() {
    for (unsigned int floor = 0; floor < HARDWARE_NUMBER_OF_FLOORS; floor++) {
        if (hardware_read_floor_sensor(floor)) {
            return floor;
        }
    }

    return FLOOR_UNDEFINED;
}

static bool fsm_elevator_is_at_a_floor(const Position position) {
    return position.floor != FLOOR_UNDEFINED && position.offset == OFFSET_AT_FLOOR;
}

static Position fsm_decide_elevator_position(const int last_floor, const HardwareMovement movement_when_left_floor) {
    int new_floor = last_floor;
    Offset offset = OFFSET_UNDEFINED;

    const int current_floor = fsm_get_current_floor();
    if (current_floor != FLOOR_UNDEFINED) {
        new_floor = current_floor;
        offset = OFFSET_AT_FLOOR;
    }

    if (offset == OFFSET_UNDEFINED && movement_when_left_floor == HARDWARE_MOVEMENT_DOWN) {
        offset = OFFSET_BELOW;
    } else if (offset == OFFSET_UNDEFINED && movement_when_left_floor == HARDWARE_MOVEMENT_UP) {
        offset = OFFSET_ABOVE;
    }

    return (Position){new_floor, offset};
}

/**
 * #################################################################################################################
 * #####                                       ORDERS                                                          #####
 * #################################################################################################################
 */

static void fsm_clear_order_lights() {
    for (unsigned int floor = 0; floor < HARDWARE_NUMBER_OF_FLOORS; floor++) {
        for (HardwareOrder order_type = HARDWARE_ORDER_UP; order_type <= HARDWARE_ORDER_DOWN; order_type++) {
            hardware_command_order_light(floor, order_type, false);
        }
    }
}

static void fsm_manage_orders_and_update_queue(Order** pp_priority_queue, const Position current_position) {
    for (unsigned int floor = 0; floor < HARDWARE_NUMBER_OF_FLOORS; floor++) {
        for (HardwareOrder order_type = HARDWARE_ORDER_UP; order_type <= HARDWARE_ORDER_DOWN; order_type++) {
            if (hardware_read_order(floor, order_type)) {
                *pp_priority_queue = priority_queue_add_order(priority_queue_order_create(floor, order_type),
                                                              *pp_priority_queue,
                                                              current_position.floor,
                                                              current_position.offset == OFFSET_AT_FLOOR);
                hardware_command_order_light(floor, order_type, true);
            }
        }
    }
}

static bool fsm_top_order_is_at_current_floor(const Order* p_priority_queue, const int floor) {
    return !priority_queue_is_empty(p_priority_queue) && p_priority_queue->floor == floor;
}

static void fsm_clear_top_order_and_update_order_lights(Order** pp_priority_queue) {
    for (unsigned int order_type = HARDWARE_ORDER_UP; order_type <= HARDWARE_ORDER_DOWN; order_type++) {
        hardware_command_order_light((*pp_priority_queue)->floor, order_type, false);
    }

    *pp_priority_queue = priority_queue_pop(*pp_priority_queue, (*pp_priority_queue)->floor);
}

/**
 * #################################################################################################################
 * #####                                       INTERRUPTS                                                      #####
 * #################################################################################################################
 */

static void fsm_sigint_handler(int sig) {
    (void)(sig);
    m_fsm_should_abort = true;
}