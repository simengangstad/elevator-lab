/**
 * @brief Implementation of the Finite State Machine
 *
 */

void fsmUpdate(State currentState) {
	// Kode her
	return;
}

void fsmUpdateLights() {
	// Kode her
	return;
}

State fsmDecideNextState(State currentState) {
	State nextState;
	if (currentState == state1 && relevantHardware->poll) {
		// Kode her
	}
	else if () {
		// etc
		nextState = example;
	}

	return nextState;
}

void fsmTransition(State currentState, State nextState) {
	// Kode her
	return;
}

void fsmStateEnter(State currentState) {
	// Kode her
	return;
}

void fsmStateExit(State currentState) {
	// Kode her
	return;
}

void fsmStateUpdate(State currentState) {
	// Kode her
	return;
}
