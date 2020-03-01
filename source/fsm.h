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

/**
 * @brief Starts the FSM.
 */
void fsm_run();

#endif
