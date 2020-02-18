#include "../priority_queue.h"

// TNODE-1
bool testMakeNode() {
	int testFloor = 3;
	HardwareOrder testDirection = HARDWARE_ORDER_UP;
	Node* testNode = nodeCreate(testFloor, testDirection);

	bool result = (testNode->floor == testFloor && testNode->direction == testDirection && !testNode->nextNode);
	free(testNode);
	return result;
}

void validateMakeNode() {
	assert(testMakeNode);
	return;
}

// TNODE-2
bool testLinkNodes() {
	Node* testNodeFirst = nodeCreate(2, HARDWARE_ORDER_DOWN);
	Node* testNodeSecond = nodeCreate(3, HARDWARE_ORDER_INSIDE);
	testNodeFirst->nextNode = testNodeSecond;
	bool result = testNodeFirst->nextNode == testNodeSecond;
	free(testNodeFirst);
	free(testNodeSecond);
	return result;
}

void validateLinkNodes() {
	assert(testLinkNodes());
	return;
}

// TNODE-3
bool testGetFunctions() {
	int testFloor = 1;
	HardwareOrder testDirection = HARDWARE_ORDER_UP;
	Node* testNode = nodeCreate(testFloor, testDirection);
	int testNodeFloor = nodeGetFloor(testNode);
	HardwareOrder testNodeDirection = nodeGetDirection(testNode);
	bool result = testFloor == testNodeFloor && testDirection == testNodeDirection;
	free(testNode);
	return result;
}

void validateGetFunctions() {
	assert(testGetFunctions);
	return;
}

// TQUEUE-1
void testQueueCreationAndAdd() {
	int currentFloor = 1;
	printf("Tests adding-algoritm based on cases\n");
	printf("Case 1: new compatible order between current floor and goal\n");
	printf("At floor %i", currentFloor, " , new order going up from floor 3.\n");
	Node* firstNode1 = nodeCreate(3, HARDWARE_ORDER_UP);
	queuePrint(firstNode1);
	printf("New order from inside to second floor\n");
	Node* secondNode1 = nodeCreate(2, HARDWARE_ORDER_INSIDE);
	firstNode1 = queueAddNode(secondNode1, firstNode1, currentFloor);
	queuePrint(firstNode1);
	print("Test successful if new order is placed on top of existing order\n");
	queueClear(firstNode1);


	printf("Case 2: new compatible order not between current floor and goal\n");
	printf("At floor %i", currentFloor, " , new order going up from floor 3.\n");
	Node* firstNode2 = nodeCreate(3, HARDWARE_ORDER_UP);
	queuePrint(firstNode2);
	printf("New order from inside to fourth floor\n");
	Node* secondNode2 = nodeCreate(4, HARDWARE_ORDER_INSIDE);
	firstNode2 = queueAddNode(secondNode2, firstNode2, currentFloor);
	queuePrint(firstNode2);
	print("Test successful if new order is placed below existing order\n");
	queueClear(firstNode2);


	printf("Case 3: new incompatible order between current floor and goal\n");
	printf("At floor %i", currentFloor, " , new order going up from floor 3.\n");
	Node* firstNode3 = nodeCreate(3, HARDWARE_ORDER_UP);
	queuePrint(firstNode3);
	printf("New order going down from second floor\n");
	Node* secondNode3 = nodeCreate(2, HARDWARE_ORDER_DOWN);
	firstNode3 = queueAddNode(secondNode3, firstNode3, currentFloor);
	queuePrint(firstNode3);
	print("Test successful if new order is placed below existing order\n");
	queueClear(firstNode3);


	printf("Case 4: deletion of duplicate orders of lower priority\n");
	printf("At floor %i", currentFloor, " , new order going up from floor 3.\n");
	Node* firstNode4 = nodeCreate(3, HARDWARE_ORDER_UP);
	queuePrint(firstNode4);
	printf("New order going down from second floor\n");
	Node* secondNode4 = nodeCreate(2, HARDWARE_ORDER_DOWN);
	firstNode4 = queueAddNode(secondNode4, firstNode4, currentFloor);
	queuePrint(firstNode4);
	print("New order going up from second floor\n");
	Node* thirdNode4 = nodeCreate(2, HARDWARE_ORDER_UP);
	firstNode4 = queueAddNode(thirdNode4, firstNode4, currentFloor);
	queuePrint(firstNode4);
	printf("Test successful if duplicate order is deleted correctly");
	queueClear(firstNode4);
}

// T_QUEUE2
void testQueueHelpFunctions() {
	int currentFloor = 1;
	printf("Help functions\n");
	Node* firstNode = nodeCreate(3, HARDWARE_ORDER_UP);
	firstNode = queueAddNode(nodeCreate(2, HARDWARE_ORDER_INSIDE, firstNode, currentFloor));
	firstNode = queueAddNode(nodeCreate(4, HARDWARE_ORDER_DOWN, firstNode, currentFloor));
	printf("Current queue:\n");
	queuePrint(firstNode);
	printf("Pop first node\n");
	firstNode = queuePop(firstNode, currentFloor);
	printf("Current queue:\n");
	queuePrint(firstNode);
	printf("Test successful if first order is removed and remaining orders are unchanged");
	queueClear(firstNode);


	currentFloor = 2;
	Node* firstNode2 = nodeCreate(2, HARDWARE_ORDER_UP);
	firstNode2 = queueAddNode(nodeCreate(4, HARDWARE_ORDER_UP, firstNode2, currentFloor));
	firstNode2 = queueAddNode(nodeCreate(3, HARDWARE_ORDER_INSIDE, firstNode2, currentFloor));
	printf("Current queue:\n");
	queuePrint(firstNode2);

	printf("Pop first node\n");
	firstNode2 = queuePop(firstNode2, currentFloor);
	printf("Current queue:\n");
	queuePrint(firstNode2);
	printf("Test successful if first order is removed and remaining orders are shuffled");
	queueClear(firstNode2);
	
	Node* firstNode3 = NULL;
	printf("Test successful if expression returns true: ", queueIsEmpty(firstNode3), "\n");
	queueClear(firstNode3);

	return;
}



void validatePriorityQueueTests() {

	assert(testMakeNode);
	assert(testLinkNodes());
	assert(testGetFunctions);
	testQueueCreationAndAdd();
	testQueueHelpFunctions();
	return;
}
