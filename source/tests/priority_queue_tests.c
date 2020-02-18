#include "../priority_queue.h"

// TNODE-1
bool testMakeNode() {
	int testFloor = 3;
	HardwareOrder testDirection = HARDWARE_ORDER_UP;
	Node* testNode = nodeCreate(testFloor, testDirection);
	return (testNode->floor == testFloor && testNode->direction == testDirection && !testNode->nextNode);
		
// TNODE-2
Node* testLinkNodes() {
	Node* testNodeFirst = nodeCreate(2, HARDWARE_ORDER_DOWN);
	Node* testNodeSecond = nodeCreate(3, HARDWARE_ORDER_INSIDE);
	testNodeFirst->nextNode = testNodeSecond;
	return testNodeFirst;
}

// TNODE-3
Node* testGetFunctions() {
	Node* testNode = nodeCreate(1, HARDWARE_ORDER_UP);
	return testNode;
}

int main() {
	{
		// TNODE-1
		Node* testNode = testMakeNode();
		assert(testNode->floor == 3);
		assert(testNode->direction == HARDWARE_ORDER_UP);
		assert(!testNode->nextNode);
		free(testNode);
	}
	
	{
		// TNODE-2
		Node* testNode = testLinkNodes();
		assert(testNode->nextNode);
		assert(!testNode->nextNode->nextNode);
		free(testNode->nextNode);
		free(testNode);
	}
	
	{
		// TNODE-3
		Node* testNode = testGetFunctions();
		assert(testNode->floor == nodeGetFloor(testNode));
		assert(testNode->direction == nodeGetDirection(testNode));
	}
	
	{
		// TQUEUE-1
		
	}
	
	return 0;
}
