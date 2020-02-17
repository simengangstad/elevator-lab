/**
 * @file
 * @brief Finite state machine controlling system.
 *
 */

/**
 * @brief Enum of the possible defined states
 */
typedef enum {
	Startup,
	Idle,
	Move,
	DoorOpen,
	Stop
} State; 


/**
 * @brief Performs "Do"-actions connected to @p currentState
 *
 * @param[in] currentState Current state of the elevator.
 */
void fsmUpdate(State currentState);

/**
 * @brief Updates all lights connected to the elevator.
 */
void fsmUpdateLights();

/**
 * @brief Polls relevant hardware and decides next state.
 *
 * @param[in] currentState Current state of the elevator
 *
 * @return Next state in the fsm based on current state of hardware.
 */
State fsmDecideNextState(State currentState);

/**
 * @brief Handles transitioning between two states, @p currentState and @p nextState.
 *
 * @param[in] currentState Current state of the elevator, the state the fsm is leaving.
 * @param[in] nextState Next state of the elevator, the state the fsm is entering.
 */
void fsmTransition(State currentState, State nextState);

/**
 * @brief Performs "Enter"-actions connected to entering @p currentState .
 *
 * @param[in] currentState State that is being entered.
 */
void fsmStateEnter(State currentState);

/**
 * @brief Performs "Exit"-actions connected to exiting @p currentState.
 *
 * @param[in] currentState State that is being exited out of.
 */
void fsmStateExit(State currentState);

/**
 * @brief ???
 *
 */
void fsmStateUpdate(State currentState);
