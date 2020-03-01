/**
 * @file  
 * 
 * @brief Implementation of the tests for the FSM module.
 */

#include "fsm_tests.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>

#include "fsm.h"
#include "test_util.h"

/**
 * @brief Will check that the FSM doesn't make a transition before the requirements for the 
 *        given transition is met.
 * 
 * @note Test TFSM-1
 * 
 * @return True if all the transitions only happens if the given requirements are met.
 */
static bool fsm_tests_check_transtion_requirements() {
    {
        // Testing the transition from startup to idle, this requires that the elevator
        // is at a floor
        printf("Testing transition from startup to idle.\n");
        State state = STATE_STARTUP;
        Position position = {-1, OFFSET_UNDEFINED};
        bool is_at_floor = false;

        while (!is_at_floor) {
            for (unsigned int floor = 0; floor < HARDWARE_NUMBER_OF_FLOORS; floor++) {
                if (hardware_read_floor_sensor(floor)) {
                    is_at_floor = true;
                    position.floor = floor;
                    position.offset = OFFSET_AT_FLOOR;
                    break;
                }
            }

            hardware_command_movement(HARDWARE_ORDER_DOWN);
        }

        hardware_command_movement(HARDWARE_MOVEMENT_STOP);

        state = fsm_decide_next_state(state, NULL, position);

        if (state != STATE_IDLE) {
            fprintf(stderr, "Did not transition to Idle from Startup. The requirements where not met\n");
            return false;
        }

        printf("OK.\n\n");
    }

    {
        // Testing the transition from idle to move, this requires that the priority queue
        // is not empty and that the stop button is not pressed
        printf("Testing transition from idle to move.\n");
        State state = STATE_IDLE;
        Position current_position = {-1, OFFSET_UNDEFINED};

        for (unsigned int floor = 0; floor < HARDWARE_NUMBER_OF_FLOORS; floor++) {
            if (hardware_read_floor_sensor(floor)) {
                current_position.floor = floor;
                break;
            }
        }

        Order* priority_queue = priority_queue_add_order(priority_queue_order_create(3, HARDWARE_ORDER_DOWN), NULL, current_position.floor, current_position.offset == OFFSET_AT_FLOOR);

        state = fsm_decide_next_state(state, priority_queue, current_position);

        if (state != STATE_MOVE) {
            fprintf(stderr, "Did not transition to Move from Idle. The requirements where not met\n");
            priority_queue_clear(priority_queue);
            return false;
        }

        printf("OK.\n\n");
    }

    {
        // Testing the transition from move to door open, this requires that the
        // elevator arrived at the queued floor
        printf("Testing transition from move to door open.\n");
        State state = STATE_MOVE;
        Position current_position = {-1, OFFSET_AT_FLOOR};
        Order* priority_queue = NULL;

        for (unsigned int floor = 0; floor < HARDWARE_NUMBER_OF_FLOORS; floor++) {
            if (hardware_read_floor_sensor(floor)) {
                priority_queue = priority_queue_add_order(priority_queue_order_create(3, HARDWARE_ORDER_DOWN),
                                                          priority_queue,
                                                          floor,
                                                          current_position.offset == OFFSET_AT_FLOOR);
                current_position.floor = floor;
                break;
            }
        }

        hardware_command_movement(HARDWARE_MOVEMENT_UP);

        const float interval_to_check = 7.0;
        time_t start_time = time(NULL);

        // Run for some time so that we are sure that we actually move to the floor.
        while (time(NULL) - start_time <= interval_to_check) {
            for (unsigned int floor = 0; floor < HARDWARE_NUMBER_OF_FLOORS; floor++) {
                if (hardware_read_floor_sensor(floor)) {
                    current_position.floor = floor;
                    break;
                }
            }

            state = fsm_decide_next_state(state, priority_queue, current_position);

            if (state == STATE_DOOR_OPEN) {
                hardware_command_movement(HARDWARE_MOVEMENT_STOP);
                break;
            }
        }

        priority_queue_clear(priority_queue);
        if (state != STATE_DOOR_OPEN) {
            fprintf(stderr, "Did not transition to door open from move. The requirements where not met\n");
            return false;
        }

        printf("OK.\n\n");
    }

    {
        // Testing the transition from door open to move, this requires that the
        // door is closed and that the  queue is not empty
        printf("Testing transition from door open to move.\n");
        State state = STATE_DOOR_OPEN;
        Order* priority_queue = priority_queue_add_order(priority_queue_order_create(0, HARDWARE_ORDER_DOWN), NULL, 1, true);
        hardware_command_door_open(0);

        state = fsm_decide_next_state(state, priority_queue, (Position){3, OFFSET_AT_FLOOR});

        priority_queue_clear(priority_queue);
        if (state != STATE_MOVE) {
            fprintf(stderr, "Did not transition to move from door open. The requirements where not met\n");
            return false;
        }

        printf("OK.\n\n");
    }

    {
        // Testing the transition from door open to idle, this requires that the
        // door is closed and that the queue is empty
        printf("Testing transition from door open to idle.\n");
        State state = STATE_DOOR_OPEN;
        hardware_command_door_open(0);
        state = fsm_decide_next_state(state, NULL, (Position){3, OFFSET_AT_FLOOR});

        if (state != STATE_IDLE) {
            fprintf(stderr, "Did not transition to idle from door open. The requirements where not met\n");
            return false;
        }

        printf("OK.\n\n");
    }

    {
        // Testing the transition from startup, idle, move and door open to stop, this
        // requires that the stop button is pressed
        printf("Testing transition from startup, idle, move and door open to stop.\n");
        printf("Keep the stop button pressed now please. Press enter when ready.\n");
        test_util_wait_until_enter_key_is_pressed();

        State state = STATE_STARTUP;
        Position current_position = {3, OFFSET_AT_FLOOR};
        state = fsm_decide_next_state(state, NULL, current_position);

        if (state != STATE_STOP) {
            fprintf(stderr, "Did not transition to stop from startup. The requirements where not met\n");
            return false;
        }

        state = STATE_IDLE;
        state = fsm_decide_next_state(state, NULL, current_position);

        if (state != STATE_STOP) {
            fprintf(stderr, "Did not transition to stop from stop. The requirements where not met\n");
            return false;
        }

        state = STATE_MOVE;
        state = fsm_decide_next_state(state, NULL, current_position);

        if (state != STATE_STOP) {
            fprintf(stderr, "Did not transition to stop from move. The requirements where not met\n");
            return false;
        }

        state = STATE_DOOR_OPEN;
        state = fsm_decide_next_state(state, NULL, current_position);

        if (state != STATE_STOP) {
            fprintf(stderr, "Did not transition to stop from door open. The requirements where not met\n");
            return false;
        }

        printf("OK.\n\n");
        printf("Release the stop button now please. Press enter when ready.\n");
        test_util_wait_until_enter_key_is_pressed();
    }

    {
        // Testing the transition from stop to idle, this
        // requires that the stop button is release
        printf("Testing transition from stop to idle.\n");

        State state = STATE_STOP;

        state = fsm_decide_next_state(state, NULL, (Position){3, OFFSET_AT_FLOOR});

        if (state != STATE_IDLE) {
            fprintf(stderr, "Did not transition to stop from startup. The requirements where not met\n");
            return false;
        }

        printf("OK.\n\n");
    }

    return true;
}

void fsm_tests_validate() {
    printf("=========== Starting FSM tests ===========\n\n");

    printf("1. Check if transitions only occur when the requirements are met (TFSM-1)\n");
    assert(fsm_tests_check_transtion_requirements());
    printf("1. Passed\n\n");

    printf("=========== FSM tests passed ===========\n\n");
}