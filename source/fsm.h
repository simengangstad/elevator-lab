/**
 * @file
 * @brief Finite state machine for the elevator.
 */

#ifndef FSM_H
#define FSM_H

/**
 * @brief Specifies an undefined floor, is used during cases when the FSM don't have information about the current
 *        location of the elevator, e.g. the startup state.
 */
#define FLOOR_UNDEFINED -1

#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "door.h"
#include "hardware.h"
#include "priority_queue.h"

// TODO: Liker egentlig ikke at dette er public, men det må jo være det for testene...

/**
 * @brief The possible states for the state machine.
 */
typedef enum {
    STATE_STARTUP,
    STATE_IDLE,
    STATE_MOVE,
    STATE_DOOR_OPEN,
    STATE_STOP,
    STATE_UNDEFINED
} State;

/**
 * @brief Specifies where the elevator is relative to a given floor.
 */
typedef enum Offset {
    OFFSET_AT_FLOOR,
    OFFSET_BELOW,
    OFFSET_ABOVE,
    OFFSET_UNDEFINED
} Offset;

/**
 * @brief Information about where the elevator is in relation to a given floor. 
 * 
 * @note If the elevator is between the 3rd and the 2nd floor going down, this 
 *       struct would hold 3rd floor at OFFSET_BELOW from the #Offset enum.
 */
typedef struct Position {
    /**
     * @brief The floor of the position.
     */
    int floor;

    /**
     * @brief The offset of the position.
     */
    Offset offset;
} Position;

/**
 * @brief Starts the FSM.
 */
void fsm_run();

/**
 * @brief Polls hardware, checks the current state and queue, and decides the next state based.
 *
 * @param[in] current_state Current state of the elevator
 * @param[in] p_priority_queue The current queue, makes it possible for states to check if 
 * 						     the queue is in a given state in order to decide the next state. 
 * @param[in] current_position The current position the elevator is at. 
 * 
 * @return Next state in the FSM based on @p current_state, @p priority_queue, @p current_position and hardware input.
 */
State fsm_decide_next_state(const State current_state, const Node* p_priority_queue, const Position current_position);

/**
 * @brief Handles transitioning between two states, @p current_state and @p next_state. Will execute the exit operations for @p current_state 
 * 		  and the enter operations for @p next_state.
 *
 * @param[in] current_state Current state of the elevator, the state the FSM is leaving.
 * @param[in] next_state Next state of the elevator, the state the FSM is entering.
 * @param[in, out] pp_priority_queue Pointer to the current queue, used to check the current state for the orders
 * 							   during the transition and for updating the queue. 
 * @param[in, out] p_movement_when_left_floor The movement the elevator was set to when it left from the last floor. 
 * @param[in] current_position The current position of the elevator. 
 */
void fsm_transition(const State current_state, const State next_state, Node** pp_priority_queue, HardwareMovement* p_movement_when_left_floor, const Position current_position);

/**
 * @brief Will execute the update function defined for the @p current_state and update
 * 		  the @p pp_priority_queue.
 * 
 * @param[in] current_state The current state that shall have its update function called. 
 * @param[in, out] pp_priority_queue The queue, makes it possible for the states to update the queue. 
 * @param[in] current_position The current position of the elevator. 
 */
void fsm_state_update(const State current_state, Node** pp_priority_queue, const Position current_position);

#endif
