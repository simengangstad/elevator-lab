/**
 * @file
 * @brief Finite state machine controlling system.
 */

#ifndef FSM_H
#define FSM_H

#include <stdbool.h>
#include <stdio.h>

#include "hardware.h"
#include "priority_queue.h"

/**
 * @brief Enum of the possible defined states
 */
typedef enum {
	Startup,
	Idle,
	Move,
	DoorOpen,
	Stop,
	Undefined
} State; 


/**
 * @brief Updates all lights connected to the elevator.
 */
void fsmUpdateLights();

/**
 * @brief Polls relevant hardware and decides next state.
 *
 * @param[in] currentState Current state of the elevator
 * @param[in] priortyQueue The current queue, makes it possible for states to check if 
 * 						   the queue is in a given state in order to decide the next state. 
 *
 * @return Next state in the fsm based on current state of hardware.
 */
State fsmDecideNextState(State currentState, const Node* priorityQueue);

/**
 * @brief Handles transitioning between two states, @p currentState and @p nextState. Will execute the exit operations for @p currentState 
 * 		  and the enter operations for @p nextState.
 *
 * @param[in] currentState Current state of the elevator, the state the fsm is leaving.
 * @param[in] nextState Next state of the elevator, the state the fsm is entering.
 */
void fsmTransition(State currentState, State nextState);

/**
 * @brief Will execute the update function defined for the @p currentState and update
 * 		  internal state.
 * 
 * @param[in] shouldClearOrders Makes it possible for states to clear the queue and prevent further enqueuements.
 * 							    This is utilized by the #Startup and #Stop state. 
 */
void fsmStateUpdate(State currentState, bool* shouldClearOrders);

#endif
