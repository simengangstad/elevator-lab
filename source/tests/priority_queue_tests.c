#include "priority_queue_tests.h"

/**
 * @brief Checks that node creation is set up correctly. 
 * 
 * @note Test TNODE-1
 * 
 * @return True if the node got create correctly. 
 */
bool priority_queue_tests_check_make_node() {
    const int test_floor = 3;
    const HardwareOrder test_direction = HARDWARE_ORDER_UP;
    Node* test_node = node_create(test_floor, test_direction);

    bool result = (test_node->floor == test_floor && test_node->direction == test_direction && !test_node->next_node);
    free(test_node);
    return result;
}

/**
 * @brief Checks that nodes get linked correctly
 *
 * @note Test TNODE-2
 *  
 * @return true if the link is correct. 
 */
bool priority_queue_tests_check_link_nodes() {
    Node* test_node_first = node_create(2, HARDWARE_ORDER_DOWN);
    Node* test_node_second = node_create(3, HARDWARE_ORDER_INSIDE);
    test_node_first->next_node = test_node_second;
    bool result = test_node_first->next_node == test_node_second;
    free(test_node_first);
    free(test_node_second);
    return result;
}

/**
 * @brief Checks that the functions for getting floor and direction from a node returns the correct result.
 *
 * @note Test TNODE-3 
 *  
 * @return true If the result of the node match with the input. 
 */
bool priority_queue_tests_check_get_function() {
    int test_floor = 1;
    HardwareOrder test_direction = HARDWARE_ORDER_UP;
    Node* test_node = node_create(test_floor, test_direction);
    bool result = test_floor == test_node->floor && test_direction == test_node->direction;
    free(test_node);
    return result;
}

/**
 * @brief Checks a range of cases with creating a queue and adding to it.
 * 
 * @note TEST TQUEUE-1
 */
void priority_queue_check_queue_creation_and_add() {
    {
        const int current_floor = 0;
        printf("Tests adding-algoritm based on cases\n\n");
        printf("Case 1: new compatible order between current floor and goal\n");
        printf("At floor %i", current_floor);
        printf(" , new order going up from floor 3.\n");
        Node* first_node = node_create(2, HARDWARE_ORDER_UP);
        queue_print(first_node);
        printf("New order from inside to second floor\n");
        Node* second_node = node_create(1, HARDWARE_ORDER_INSIDE);
        first_node = queue_add_node(second_node, first_node, current_floor, true);
        queue_print(first_node);
        printf("Test successful if new order is placed on top of existing order\n\n");
        queue_clear(first_node);
    }

    {
        const int current_floor = 0;
        printf("Case 2: new compatible order not between current floor and goal\n\n");
        printf("At floor %i", current_floor);
        printf(" , new order going up from floor 3.\n");
        Node* first_node = node_create(2, HARDWARE_ORDER_UP);
        queue_print(first_node);
        printf("New order from inside to fourth floor\n");
        Node* second_node = node_create(3, HARDWARE_ORDER_INSIDE);
        first_node = queue_add_node(second_node, first_node, current_floor, true);
        queue_print(first_node);
        printf("Test successful if new order is placed below existing order\n\n");
        queue_clear(first_node);
    }

    {
        const int current_floor = 0;
        printf("Case 3: new incompatible order between current floor and goal\n\n");
        printf("At floor %i", current_floor);
        printf(" , new order going up from floor 3.\n");
        Node* first_node = node_create(2, HARDWARE_ORDER_UP);
        queue_print(first_node);
        printf("New order going down from second floor\n");
        Node* second_node = node_create(1, HARDWARE_ORDER_DOWN);
        first_node = queue_add_node(second_node, first_node, current_floor, true);
        queue_print(first_node);
        printf("Test successful if new order is placed below existing order\n\n");
        queue_clear(first_node);
    }

    {
        const int current_floor = 0;
        printf("Case 4: deletion of duplicate orders of lower priority\n\n");
        printf("At floor %i", current_floor);
        printf(" , new order going up from floor 3.\n");
        Node* first_node = node_create(2, HARDWARE_ORDER_UP);
        queue_print(first_node);
        printf("New order going down from second floor\n");
        Node* second_node = node_create(1, HARDWARE_ORDER_DOWN);
        first_node = queue_add_node(second_node, first_node, current_floor, true);
        queue_print(first_node);
        printf("New order going up from second floor\n");
        Node* third_node = node_create(1, HARDWARE_ORDER_UP);
        first_node = queue_add_node(third_node, first_node, current_floor, true);
        queue_print(first_node);
        printf("Test successful if duplicate order is deleted correctly\n\n");
        queue_clear(first_node);
    }
}

/**
 * @brief Will check the helper functions for the queue.
 * 
 * @note TEST TQUEUE-2
 */
void priority_queue_tests_check_queue_help_functions() {
    printf("Help functions\n");

    {
        const int current_floor = 0;
        Node* first_node = node_create(2, HARDWARE_ORDER_UP);
        first_node = queue_add_node(node_create(1, HARDWARE_ORDER_INSIDE), first_node, current_floor, true);
        first_node = queue_add_node(node_create(3, HARDWARE_ORDER_DOWN), first_node, current_floor, true);
        printf("Current queue:\n");
        queue_print(first_node);
        printf("Pop first node\n");
        first_node = queue_pop(first_node, current_floor);
        printf("Current queue:\n");
        queue_print(first_node);
        printf("Test successful if first order is removed and remaining orders are unchanged\n\n");
        queue_clear(first_node);
    }

    {
        const int current_floor = 1;
        Node* first_node = node_create(1, HARDWARE_ORDER_UP);
        first_node = queue_add_node(node_create(3, HARDWARE_ORDER_UP), first_node, current_floor, true);
        first_node = queue_add_node(node_create(2, HARDWARE_ORDER_INSIDE), first_node, current_floor, true);
        printf("Current queue:\n");
        queue_print(first_node);

        printf("Pop first node\n");
        first_node = queue_pop(first_node, current_floor);
        printf("Current queue:\n");
        queue_print(first_node);
        printf("Test successful if first order is removed and remaining orders are shuffled\n\n");
        queue_clear(first_node);
    }

    {
        Node* first_node = NULL;
        printf("Test successful if expression returns true: %i\n\n", queue_is_empty(first_node));
    }

    return;
}

void priority_queue_tests_validate() {
    printf("=========== Starting Queue tests ===========\n\n");
    printf("Test 1: Test that makeNode() works\n");
    assert(priority_queue_tests_check_make_node());
    printf("Test 1 succesful if passed to here.\n");
    printf("\n");

    printf("Test 2: Test that linking nodes works\n");
    assert(priority_queue_tests_check_link_nodes());
    printf("Test 2 succesful if passed to here.\n");
    printf("\n");

    printf("Test 3: Test that get-functions work\n");
    assert(priority_queue_tests_check_get_function());
    printf("Test 3 succesful if passed to here.\n");
    printf("\n");

    printf("Test 4: Testing creation of queue and adding nodes\n");
    priority_queue_check_queue_creation_and_add();
    printf("\n");

    printf("Test 5: Testing queue help-functions\n");
    priority_queue_tests_check_queue_help_functions();
    printf("\n");

    printf("================== Queue test complete =================\n");

    return;
}
