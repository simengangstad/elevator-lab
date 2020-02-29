#include "priority_queue_tests.h"

/**
 * @brief Checks that order creation is set up correctly. 
 * 
 * @note Test TORDER-1
 * 
 * @return True if the order got create correctly. 
 */
bool priority_queue_tests_check_make_order() {
    const int test_floor = 3;
    const HardwareOrder test_direction = HARDWARE_ORDER_UP;
    Order* test_order = order_create(test_floor, test_direction);

    bool result = (test_order->floor == test_floor && test_order->direction == test_direction && !test_order->next_order);
    free(test_order);
    return result;
}

/**
 * @brief Checks that orders get linked correctly
 *
 * @note Test TORDER-2
 *  
 * @return true if the link is correct. 
 */
bool priority_queue_tests_check_link_orders() {
    Order* test_order_first = order_create(2, HARDWARE_ORDER_DOWN);
    Order* test_order_second = order_create(3, HARDWARE_ORDER_INSIDE);
    test_order_first->next_order = test_order_second;
    bool result = test_order_first->next_order == test_order_second;
    free(test_order_first);
    free(test_order_second);
    return result;
}

/**
 * @brief Checks that the functions for getting floor and direction from a order returns the correct result.
 *
 * @note Test TORDER-3 
 *  
 * @return true If the result of the order match with the input. 
 */
bool priority_queue_tests_check_get_function() {
    int test_floor = 1;
    HardwareOrder test_direction = HARDWARE_ORDER_UP;
    Order* test_order = order_create(test_floor, test_direction);
    bool result = test_floor == test_order->floor && test_direction == test_order->direction;
    free(test_order);
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
        Order* first_order = order_create(2, HARDWARE_ORDER_UP);
        queue_print(first_order);
        printf("New order from inside to second floor\n");
        Order* second_order = order_create(1, HARDWARE_ORDER_INSIDE);
        first_order = queue_add_order(second_order, first_order, current_floor, true);
        queue_print(first_order);
        printf("Test successful if new order is placed on top of existing order\n\n");
        queue_clear(first_order);
    }

    {
        const int current_floor = 0;
        printf("Case 2: new compatible order not between current floor and goal\n\n");
        printf("At floor %i", current_floor);
        printf(" , new order going up from floor 3.\n");
        Order* first_order = order_create(2, HARDWARE_ORDER_UP);
        queue_print(first_order);
        printf("New order from inside to fourth floor\n");
        Order* second_order = order_create(3, HARDWARE_ORDER_INSIDE);
        first_order = queue_add_order(second_order, first_order, current_floor, true);
        queue_print(first_order);
        printf("Test successful if new order is placed below existing order\n\n");
        queue_clear(first_order);
    }

    {
        const int current_floor = 0;
        printf("Case 3: new incompatible order between current floor and goal\n\n");
        printf("At floor %i", current_floor);
        printf(" , new order going up from floor 3.\n");
        Order* first_order = order_create(2, HARDWARE_ORDER_UP);
        queue_print(first_order);
        printf("New order going down from second floor\n");
        Order* second_order = order_create(1, HARDWARE_ORDER_DOWN);
        first_order = queue_add_order(second_order, first_order, current_floor, true);
        queue_print(first_order);
        printf("Test successful if new order is placed below existing order\n\n");
        queue_clear(first_order);
    }

    {
        const int current_floor = 0;
        printf("Case 4: deletion of duplicate orders of lower priority\n\n");
        printf("At floor %i", current_floor);
        printf(" , new order going up from floor 3.\n");
        Order* first_order = order_create(2, HARDWARE_ORDER_UP);
        queue_print(first_order);
        printf("New order going down from second floor\n");
        Order* second_order = order_create(1, HARDWARE_ORDER_DOWN);
        first_order = queue_add_order(second_order, first_order, current_floor, true);
        queue_print(first_order);
        printf("New order going up from second floor\n");
        Order* third_order = order_create(1, HARDWARE_ORDER_UP);
        first_order = queue_add_order(third_order, first_order, current_floor, true);
        queue_print(first_order);
        printf("Test successful if duplicate order is deleted correctly\n\n");
        queue_clear(first_order);
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
        Order* first_order = order_create(2, HARDWARE_ORDER_UP);
        first_order = queue_add_order(order_create(1, HARDWARE_ORDER_INSIDE), first_order, current_floor, true);
        first_order = queue_add_order(order_create(3, HARDWARE_ORDER_DOWN), first_order, current_floor, true);
        printf("Current queue:\n");
        queue_print(first_order);
        printf("Pop first order\n");
        first_order = queue_pop(first_order, current_floor);
        printf("Current queue:\n");
        queue_print(first_order);
        printf("Test successful if first order is removed and remaining orders are unchanged\n\n");
        queue_clear(first_order);
    }

    {
        const int current_floor = 1;
        Order* first_order = order_create(1, HARDWARE_ORDER_UP);
        first_order = queue_add_order(order_create(3, HARDWARE_ORDER_UP), first_order, current_floor, true);
        first_order = queue_add_order(order_create(2, HARDWARE_ORDER_INSIDE), first_order, current_floor, true);
        printf("Current queue:\n");
        queue_print(first_order);

        printf("Pop first order\n");
        first_order = queue_pop(first_order, current_floor);
        printf("Current queue:\n");
        queue_print(first_order);
        printf("Test successful if first order is removed and remaining orders are shuffled\n\n");
        queue_clear(first_order);
    }

    {
        Order* first_order = NULL;
        printf("Test successful if expression returns true: %i\n\n", queue_is_empty(first_order));
    }

    return;
}

void priority_queue_tests_validate() {
    printf("=========== Starting Queue tests ===========\n\n");
    printf("1. Test that makeorder() works\n");
    assert(priority_queue_tests_check_make_order());
    printf("1. Passed\n");
    printf("\n");

    printf("2. Test that linking orders works\n");
    assert(priority_queue_tests_check_link_orders());
    printf("2. Passed\n");
    printf("\n");

    printf("3. Test that get-functions work\n");
    assert(priority_queue_tests_check_get_function());
    printf("3. Passed\n");
    printf("\n");

    printf("4. Testing creation of queue and adding orders\n");
    priority_queue_check_queue_creation_and_add();
    printf("4. End\n");

    printf("5. Testing queue help-functions\n");
    priority_queue_tests_check_queue_help_functions();
    printf("5. End\n");

    printf("================== Queue test complete =================\n");

    return;
}
