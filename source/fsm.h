/**
 * @file
 * @brief Finite state machine controlling system.
 */

#ifndef FSM_H
#define FSM_H
#define FLOOR_UNDEFINED -1

#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "door.h"
#include "hardware.h"
#include "priority_queue.h"

/**
 * @brief Enum of the possible defined states
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
 * @brief Specifies where the elevator is in relative to a given floor.
 */
typedef enum Offset {
    OFFSET_AT_FLOOR,
    OFFSET_BELOW,
    OFFSET_ABOVE,
    OFFSET_UNDEFINED
} Offset;

/**
 * @brief Information about where the elevator is in relation to a given floor. E.g. between 3rd and 2nd floor going down
 *        would be 3rd floor at #Position::BELOW.
 */
typedef struct Position {
    int floor;
    Offset offset;
} Position;

/**
 * @brief Polls relevant hardware, checks the current state and queue, and decides next state.
 *
 * @param[in] current_state Current state of the elevator
 * @param[in] p_priority_queue The current queue, makes it possible for states to check if 
 * 						     the queue is in a given state in order to decide the next state. 
 * @param[in] current_position The current position the elevator is at. 
 * 
 * @return Next state in the fsm based on current state of hardware.
 */
State fsm_decide_next_state(const State current_state, const Node* p_priority_queue, const Position current_position);

/**
 * @brief Handles transitioning between two states, @p current_state and @p next_state. Will execute the exit operations for @p current_state 
 * 		  and the enter operations for @p next_state.
 *
 * @param[in] current_state Current state of the elevator, the state the fsm is leaving.
 * @param[in] next_state Next state of the elevator, the state the fsm is entering.
 * @param[in, out] pp_priority_queue Pointer to the current queue, used to check the current state for the orders
 * 							   during the transition and update the queue. 
 * @param[in, out] p_movement_when_left_floor The movement the elevator was set to when it left from the last floor. 
 * @param[in] current_position The current position the elevator is at. 
 */
void fsm_transition(const State current_state, const State next_state, Node** pp_priority_queue, HardwareMovement* p_movement_when_left_floor, const Position current_position);

/**
 * @brief Will execute the update function defined for the @p current_state and update
 * 		  internal state and the @p pp_priority_queue.
 * 
 * @param[in] current_state The current state that shall have its update function called. 
 * @param[in] pp_priority_queue The queue, makes it possible for the states to update the queue. 
 * @param[in] current_position The current position the elevator is at. 
 */
void fsm_state_update(const State current_state, Node** pp_priority_queue, const Position current_position);

#endif
