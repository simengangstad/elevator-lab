/**
 * @file
 * @brief Implementation of the Priority Queue.
 */

#include "priority_queue.h"

int nodeGetFloor(Node* nodePtr) {
	return nodePtr->floor;
}

HardwareOrder nodeGetDirection(Node* nodePtr) {
	return nodePtr->direction;
}

Node* queueAddNode(Node* newNodePtr, Node* firstNodePtr, int currentFloor) {
	// Nødvendig?
	if (!newNodePtr) {
		return NULL;
	}


	// Algorithm for adding node to queue
	Node* newFirstNodePtr = NULL;
	if (!firstNodePtr) {		// If the queue is empty:
		newNodePtr->nextNode = NULL;
		newFirstNodePtr = newNodePtr;
	}
	else {
		if ((((currentFloor < newNodePtr->floor) && (newNodePtr->floor < firstNodePtr->floor)) || ((currentFloor > newNodePtr->floor) && (newNodePtr->floor > firstNodePtr->floor))) && ((newNodePtr->direction == HARDWARE_ORDER_INSIDE) || (newNodePtr->direction == firstNodePtr->direction))) {		// If the new order is compatible as a layover: put it at the top.
			newNodePtr->nextNode = firstNodePtr;
			newFirstNodePtr = newNodePtr;
		}
		else {
			// If the new order is incompatible as a layover: put it at the bottom.
			newNodePtr->nextNode = NULL;
			newFirstNodePtr = firstNodePtr;

			currentNodePtr = newFirstNodePtr;
			while (currentNodePtr->nextNode != NULL) {
				currentNodePtr = currentNodePtr->nextNode
			}
			currentNodePtr->nextNode = newNodePtr;
		}
		

		// Traverse queue, delete orders to duplicate floors
		bool orderOnFloor[4] = {false, false, false, false}; 
		Node* currentNodePtr = newFirstNodePtr;
		Node* prevNodePtr = NULL;
		orderOnFloor[currentNodePtr->floor] = true;
		while (currentNodePtr != NULL) {
			if (orderOnFloor[currentNodePtr->floor]) {
				prevNodePtr->nextNode = currentNodePtr->nextNode;
				prevNodePtr = prevNodePtr;
			}
			else {
				orderOnFloor[currentNodePtr->floor] = true;
				prevNodePtr = currentNodePtr;
			} 
			currentNodePtr = currentNodePtr->nextNode;
		}
		// Delete orderOnFloor?
	}
	return newFirstNodePtr;
}

Node* queuePop(Node* firstNodePtr) {
	Node* tempNodePtr = firstNodePtr->nextNode;
	// free firstNodePtr;
	return tempNodePtr;
}

void queueClear(Node* firstNodePtr) {
	// free Nodes allocated?? trenger jo egn ikke.
	return;
}

bool queueIsEmpty(Node* firstNodePtr) {
	return !firstNodePtr;
}
