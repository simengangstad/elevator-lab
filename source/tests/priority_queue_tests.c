#include "priority_queue_tests.h"

// TNODE-1
bool testMakeNode() {
    int testFloor = 3;
    HardwareOrder testDirection = HARDWARE_ORDER_UP;
    Node* testNode = node_create(testFloor, testDirection);

    bool result = (testNode->floor == testFloor && testNode->direction == testDirection && !testNode->next_node);
    free(testNode);
    return result;
}

void validateMakeNode() {
    assert(testMakeNode());
    return;
}

// TNODE-2
bool testLinkNodes() {
    Node* testNodeFirst = node_create(2, HARDWARE_ORDER_DOWN);
    Node* testNodeSecond = node_create(3, HARDWARE_ORDER_INSIDE);
    testNodeFirst->next_node = testNodeSecond;
    bool result = testNodeFirst->next_node == testNodeSecond;
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
    Node* testNode = node_create(testFloor, testDirection);
    int testNodeFloor = testNode->floor;
    HardwareOrder testNodeDirection = testNode->direction;
    bool result = testFloor == testNodeFloor && testDirection == testNodeDirection;
    free(testNode);
    return result;
}

void validateGetFunctions() {
    assert(testGetFunctions());
    return;
}

// TQUEUE-1
void testQueueCreationAndAdd() {
    int currentFloor = 0;
    printf("Tests adding-algoritm based on cases\n");
    printf("Case 1: new compatible order between current floor and goal\n");
    printf("At floor %i", currentFloor);
    printf(" , new order going up from floor 3.\n");
    Node* firstNode1 = node_create(2, HARDWARE_ORDER_UP);
    queue_print(firstNode1);
    printf("New order from inside to second floor\n");
    Node* secondNode1 = node_create(1, HARDWARE_ORDER_INSIDE);
    firstNode1 = queue_add_node(secondNode1, firstNode1, currentFloor, true);
    queue_print(firstNode1);
    printf("Test successful if new order is placed on top of existing order\n");
    queue_clear(firstNode1);

    printf("Case 2: new compatible order not between current floor and goal\n");
    printf("At floor %i", currentFloor);
    printf(" , new order going up from floor 3.\n");
    Node* firstNode2 = node_create(2, HARDWARE_ORDER_UP);
    queue_print(firstNode2);
    printf("New order from inside to fourth floor\n");
    Node* secondNode2 = node_create(3, HARDWARE_ORDER_INSIDE);
    firstNode2 = queue_add_node(secondNode2, firstNode2, currentFloor, true);
    queue_print(firstNode2);
    printf("Test successful if new order is placed below existing order\n");
    queue_clear(firstNode2);

    printf("Case 3: new incompatible order between current floor and goal\n");
    printf("At floor %i", currentFloor);
    printf(" , new order going up from floor 3.\n");
    Node* firstNode3 = node_create(2, HARDWARE_ORDER_UP);
    queue_print(firstNode3);
    printf("New order going down from second floor\n");
    Node* secondNode3 = node_create(1, HARDWARE_ORDER_DOWN);
    firstNode3 = queue_add_node(secondNode3, firstNode3, currentFloor, true);
    queue_print(firstNode3);
    printf("Test successful if new order is placed below existing order\n");
    queue_clear(firstNode3);

    printf("Case 4: deletion of duplicate orders of lower priority\n");
    printf("At floor %i", currentFloor);
    printf(" , new order going up from floor 3.\n");
    Node* firstNode4 = node_create(2, HARDWARE_ORDER_UP);
    queue_print(firstNode4);
    printf("New order going down from second floor\n");
    Node* secondNode4 = node_create(1, HARDWARE_ORDER_DOWN);
    firstNode4 = queue_add_node(secondNode4, firstNode4, currentFloor, true);
    queue_print(firstNode4);
    printf("New order going up from second floor\n");
    Node* thirdNode4 = node_create(1, HARDWARE_ORDER_UP);
    firstNode4 = queue_add_node(thirdNode4, firstNode4, currentFloor, true);
    queue_print(firstNode4);
    printf("Test successful if duplicate order is deleted correctly");
    queue_clear(firstNode4);
}

// T_QUEUE2
void testQueueHelpFunctions() {
    int currentFloor = 0;
    printf("Help functions\n");
    Node* firstNode = node_create(2, HARDWARE_ORDER_UP);
    firstNode = queue_add_node(node_create(1, HARDWARE_ORDER_INSIDE), firstNode, currentFloor, true);
    firstNode = queue_add_node(node_create(3, HARDWARE_ORDER_DOWN), firstNode, currentFloor, true);
    printf("Current queue:\n");
    queue_print(firstNode);
    printf("Pop first node\n");
    firstNode = queue_pop(firstNode, currentFloor);
    printf("Current queue:\n");
    queue_print(firstNode);
    printf("Test successful if first order is removed and remaining orders are unchanged\n");
    queue_clear(firstNode);

    currentFloor = 1;
    Node* firstNode2 = node_create(1, HARDWARE_ORDER_UP);
    firstNode2 = queue_add_node(node_create(3, HARDWARE_ORDER_UP), firstNode2, currentFloor, true);
    firstNode2 = queue_add_node(node_create(2, HARDWARE_ORDER_INSIDE), firstNode2, currentFloor, true);
    printf("Current queue:\n");
    queue_print(firstNode2);

    printf("Pop first node\n");
    firstNode2 = queue_pop(firstNode2, currentFloor);
    printf("Current queue:\n");
    queue_print(firstNode2);
    printf("Test successful if first order is removed and remaining orders are shuffled\n");
    queue_clear(firstNode2);

    Node* firstNode3 = NULL;
    printf("Test successful if expression returns true: %i", queue_is_empty(firstNode3));
    printf("\n");
    queue_clear(firstNode3);

    return;
}

void priorityQueueTestsValidate() {
    printf("=========== Starting Queue tests ===========\n\n");
    printf("Test 1: Test that makeNode() works\n");
    assert(testMakeNode());
    printf("Test 1 succesful if passed to here.\n");
    printf("\n");

    printf("Test 2: Test that linking nodes works\n");
    assert(testLinkNodes());
    printf("Test 2 succesful if passed to here.\n");
    printf("\n");

    printf("Test 3: Test that get-functions work\n");
    assert(testGetFunctions());
    printf("Test 3 succesful if passed to here.\n");
    printf("\n");

    printf("Test 4: Testing creation of queue and adding nodes\n");
    testQueueCreationAndAdd();
    printf("\n");

    printf("Test 5: Testing queue help-functions\n");
    testQueueHelpFunctions();
    printf("\n");

    printf("================== Queue test complete =================\n");

    return;
}
