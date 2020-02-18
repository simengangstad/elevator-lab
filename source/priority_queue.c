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
		// Delete orderOnFloor here?
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
		remainingNodesPtr = remainingNodesPtr->next;
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
		printf("%i", n, ". order\n");
		printf("\t Floor: %i", tempNodePtr->floor, "\n");
		printf("\t Direction: %i", tempNodePtr->direction, "\n");
		n++;
	}
	printf("End of queue\n");
	return;
}
