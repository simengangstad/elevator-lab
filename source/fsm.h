/**
 * @file
 * @brief Finite state machine controlling system.
 */

#ifndef FSM_H
#define FSM_H

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
    STARTUP,
    IDLE,
    MOVE,
    DOOR_OPEN,
    STOP,
    UNDEFINED
} State;

/**
 * @brief Polls relevant hardware, checks the current state and queue, and decides next state.
 *
 * @param[in] current_state Current state of the elevator
 * @param[in] p_priority_queue The current queue, makes it possible for states to check if 
 * 						     the queue is in a given state in order to decide the next state. 
 * @param[in] current_floor The current floor the elevator is at. If the elevator is between floors this argument 
 * 						    should be the last floor the elevator was at. 
 * @return Next state in the fsm based on current state of hardware.
 */
State fsm_decide_next_state(const State current_state, const Node* p_priority_queue, const int current_floor);

/**
 * @brief Handles transitioning between two states, @p current_state and @p next_state. Will execute the exit operations for @p current_state 
 * 		  and the enter operations for @p next_state.
 *
 * @param[in] current_state Current state of the elevator, the state the fsm is leaving.
 * @param[in] next_state Next state of the elevator, the state the fsm is entering.
 * @param[in, out] pp_priority_queue Pointer to the current queue, used to check the current state for the orders
 * 							   during the transition and update the queue. 
 * @param[in] current_floor The current floor the elevator is at (if the elevator is between floors this
 * 							argument should be the last floor). 
 */
void fsm_transition(const State current_state, const State next_state, Node** pp_priority_queue, const int current_floor);

/**
 * @brief Will execute the update function defined for the @p current_state and update
 * 		  internal state.
 * 
 * @param[in] current_state The current state that shall have its update function called. 
 * @param[in] p_should_clear_orders Makes it possible for states to clear the queue and prevent further enqueuements.
 * 							        This is utilized by the #Startup and #Stop state. 
 */
void fsmStateUpdate(const State current_state, bool* p_should_clear_orders);

#endif
