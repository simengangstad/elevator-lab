/**
 * @file  
 * 
 * @brief Implementation of the tests for the FSM module.
 */

#include "fsm_tests.h"

/**
 * @brief Will check that the FSM doesn't make a transition before the requirements for the 
 *        given transition is met.
 * 
 * @note Test TFSM-1
 * 
 * @return True if all the transitions only happens if the given requirements are met.
 */
static bool fsmTestTransitionRequirements() {
    {
        printf("Testing transition from startup to idle.\n");
        // Testing the transition from startup to idle, this requires that the elevator
        // is at a floor
        State state = STARTUP;
        bool isAtFloor = false;

        while (!isAtFloor) {
            for (unsigned int floor = 0; floor < HARDWARE_NUMBER_OF_FLOORS; floor++) {
                if (hardware_read_floor_sensor(floor)) {
                    isAtFloor = true;
                    break;
                }
            }

            hardware_command_movement(HARDWARE_ORDER_DOWN);
        }

        hardware_command_movement(HARDWARE_MOVEMENT_STOP);

        state = fsm_decide_next_state(state, NULL, -1);

        if (state != IDLE) {
            fprintf(stderr, "Did not transition to Idle from Startup. The requirements where not met\n");
            return false;
        }

        printf("OK.\n\n");
    }

    {
        printf("Testing transition from idle to move.\n");
        // Testing the transition from idle to move, this requires that the priority queue
        // is not empty and that the stop button is not pressed
        State state = IDLE;
        unsigned int currentFloor = -1;

        for (unsigned int floor = 0; floor < HARDWARE_NUMBER_OF_FLOORS; floor++) {
            if (hardware_read_floor_sensor(floor)) {
                currentFloor = floor;
                break;
            }
        }

        Node* priorityQueue = queue_add_node(node_create(3, HARDWARE_ORDER_DOWN), NULL, currentFloor);

        state = fsm_decide_next_state(state, priorityQueue, currentFloor);

        if (state != MOVE) {
            fprintf(stderr, "Did not transition to Move from Idle. The requirements where not met\n");
            queue_clear(priorityQueue);
            return false;
        }

        printf("OK.\n\n");
    }

    {
        printf("Testing transition from move to door open.\n");
        // Testing the transition from move to door open, this requires that the
        // elevator arrived at the queued floor
        State state = MOVE;
        unsigned int currentFloor = -1;
        Node* priorityQueue = NULL;

        for (unsigned int floor = 0; floor < HARDWARE_NUMBER_OF_FLOORS; floor++) {
            if (hardware_read_floor_sensor(floor)) {
                priorityQueue = queue_add_node(node_create(3, HARDWARE_ORDER_DOWN), priorityQueue, floor);
                currentFloor = floor;
                break;
            }
        }

        hardware_command_movement(HARDWARE_MOVEMENT_UP);

        const float intervalToCheck = 7.0;
        time_t startTime = time(NULL);

        // Run for some time so that we are sure that we actually move to the floor.
        while (time(NULL) - startTime <= intervalToCheck) {
            for (unsigned int floor = 0; floor < HARDWARE_NUMBER_OF_FLOORS; floor++) {
                if (hardware_read_floor_sensor(floor)) {
                    currentFloor = floor;
                    break;
                }
            }

            state = fsm_decide_next_state(state, priorityQueue, currentFloor);

            if (state == DOOR_OPEN) {
                hardware_command_movement(HARDWARE_MOVEMENT_STOP);
                break;
            }
        }

        queue_clear(priorityQueue);
        if (state != DOOR_OPEN) {
            fprintf(stderr, "Did not transition to door open from move. The requirements where not met\n");
            return false;
        }

        printf("OK.\n\n");
    }

    {
        printf("Testing transition from door open to move.\n");
        // Testing the transition from door open to move, this requires that the
        // door is closed and that the  queue is not empty
        State state = DOOR_OPEN;
        Node* priorityQueue = queue_add_node(node_create(0, HARDWARE_ORDER_DOWN), NULL, 1);
        hardware_command_door_open(0);

        state = fsm_decide_next_state(state, priorityQueue, 3);

        queue_clear(priorityQueue);
        if (state != MOVE) {
            fprintf(stderr, "Did not transition to move from door open. The requirements where not met\n");
            return false;
        }

        printf("OK.\n\n");
    }

    {
        printf("Testing transition from door open to idle.\n");
        // Testing the transition from door open to idle, this requires that the
        // door is closed and that the queue is empty
        State state = DOOR_OPEN;
        hardware_command_door_open(0);
        state = fsm_decide_next_state(state, NULL, 3);

        if (state != IDLE) {
            fprintf(stderr, "Did not transition to idle from door open. The requirements where not met\n");
            return false;
        }

        printf("OK.\n\n");
    }

    {
        printf("Testing transition from startup, idle, move and door open to stop.\n");
        // Testing the transition from startup, idle, move and door open to stop, this
        // requires that the stop button is pressed

        printf("Keep the stop button pressed now please. Press enter when ready.\n");
        test_util_wait_until_enter_key_is_pressed();

        State state = STARTUP;
        state = fsm_decide_next_state(state, NULL, 3);

        if (state != STOP) {
            fprintf(stderr, "Did not transition to stop from startup. The requirements where not met\n");
            return false;
        }

        state = IDLE;
        state = fsm_decide_next_state(state, NULL, 3);

        if (state != STOP) {
            fprintf(stderr, "Did not transition to stop from stop. The requirements where not met\n");
            return false;
        }

        state = MOVE;
        state = fsm_decide_next_state(state, NULL, 3);

        if (state != STOP) {
            fprintf(stderr, "Did not transition to stop from move. The requirements where not met\n");
            return false;
        }

        state = DOOR_OPEN;
        state = fsm_decide_next_state(state, NULL, 3);

        if (state != STOP) {
            fprintf(stderr, "Did not transition to stop from door open. The requirements where not met\n");
            return false;
        }

        printf("OK.\n\n");
        printf("Release the stop button now please. Press enter when ready.\n");
        test_util_wait_until_enter_key_is_pressed();
    }

    {
        printf("Testing transition from stop to idle.\n");
        // Testing the transition from stop to idle, this
        // requires that the stop button is release

        State state = STOP;

        state = fsm_decide_next_state(state, NULL, 3);

        if (state != IDLE) {
            fprintf(stderr, "Did not transition to stop from startup. The requirements where not met\n");
            return false;
        }

        printf("OK.\n\n");
    }

    return true;
}

void fsmTestsValidate() {
    printf("=========== Starting FSM tests ===========\n\n");

    printf("1. Check if transitions only occur when the requirements are met (TFSM-1)\n");
    assert(fsmTestTransitionRequirements());
    printf("1. Passed\n\n");

    printf("=========== FSM tests passed ===========\n\n");
}