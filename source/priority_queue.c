/**
 * @file
 * @brief Implementation of the Priority Queue.
 */

#include "priority_queue.h"

Node* nodeCreate(int newFloor, HardwareOrder newDirection) {
	Node* newNodePtr = NULL;
	newNodePtr = (Node*)malloc(sizeof(Node));
	
	newNodePtr->floor = newFloor;
	newNodePtr->direction = newDirection;
	newNodePtr->nextNode = NULL;

	return newNodePtr;
}


void nodeDelete(Node* nodePtr) {
	free(nodePtr);
	return;
}

int nodeGetFloor(Node* nodePtr) {
	return nodePtr->floor;
}

HardwareOrder nodeGetDirection(Node* nodePtr) {
	return nodePtr->direction;
}

Node* queueAddNode(Node* newNodePtr, Node* firstNodePtr, int currentFloor) {
	// Nødvendig?
	if (!newNodePtr) {
		return firstNodePtr;
	}


	Node* newFirstNodePtr = NULL;


	// Algorithm for adding node to queue
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

			Node* currentNodePtr = newFirstNodePtr;
			while (currentNodePtr->nextNode != NULL) {
				currentNodePtr = currentNodePtr->nextNode;
			}
			currentNodePtr->nextNode = newNodePtr;
		}
		

		// Traverse queue, delete orders to duplicate floors
		bool orderOnFloor[4] = {false, false, false, false}; 		// Lite dynamisk, pls fix
		Node* currentNodePtr = newFirstNodePtr;
		Node* prevNodePtr = nodeCreate(0, HARDWARE_ORDER_INSIDE);		// LnodeCreate(0, HARDWARE_ORDER_INSIDE);		// Lite dynamisk dette her ass, men for å unngå sigfaultt
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
		free(prevNodePtr);
	}
	return newFirstNodePtr;
}

Node* queuePop(Node* firstNodePtr, int currentFloor) {
	Node* newFirstNodePtr = firstNodePtr->nextNode;
	free(firstNodePtr);

	// Seperate current goal from older orders, reprioritize by going through older orders and adding them to queue again.
	Node* remainingNodesPtr = newFirstNodePtr->nextNode;
	newFirstNodePtr->nextNode = NULL;
	while(!remainingNodesPtr) {
		Node* tempNodePtr = remainingNodesPtr;
		tempNodePtr->nextNode = NULL;
		newFirstNodePtr = queueAddNode(tempNodePtr, newFirstNodePtr, currentFloor);
		remainingNodesPtr = remainingNodesPtr->nextNode;
	}

	return newFirstNodePtr;
}

void queueClear(Node* firstNodePtr) {
	Node* traverseNodePtr = firstNodePtr;
	while (!traverseNodePtr) {
		Node* tempNodePtr = traverseNodePtr;
		traverseNodePtr = traverseNodePtr->nextNode;
		free(tempNodePtr);
	}
	return;
}

bool queueIsEmpty(Node* firstNodePtr) {
	return !firstNodePtr;
}

void queuePrint(Node* firstNodePtr) {
	Node* tempNodePtr = firstNodePtr;
	int n = 1;
	while(!tempNodePtr) {
		printf("%i", n);
		printf(". order\n");
		printf("\t Floor: %i", tempNodePtr->floor);
		printf("\n");
		printf("\t Direction: %i", (int) tempNodePtr->direction);
		printf("\n");
		n++;
	}
	printf("End of queue\n");
	return;
}
