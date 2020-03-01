/**
 * @file
 * @brief Implementation of the priority queue.
 */

#include "priority_queue.h"

/**
 * @brief Get the last order in @p p_priority_queue. 
 * 
 * @param [in] p_priority_queue The queue to traverse.
 * 
 * @return The last order in the @p p_priority_queue. 
 */
static Order* priority_queue_get_last_order(Order* p_priority_queue) {
    Order* p_current_order = p_priority_queue;
    while (p_current_order->next_order) {
        p_current_order = p_current_order->next_order;
    }

    return p_current_order;
}

/**
 * @brief Remove duplicate orders in the @p p_priority_queue. 
 * 
 * @param [in, out] p_priority_queue The queue to remove the duplicates from.
 */
static void priority_queue_remove_duplicate_orders(Order* p_priority_queue) {
    bool order_on_floor[4] = {false, false, false, false};
    Order* p_current_order = p_priority_queue;
    Order* p_previous_order = NULL;

    while (p_current_order) {
        if (order_on_floor[p_current_order->floor]) {
            p_previous_order->next_order = p_current_order->next_order;
            free(p_current_order);
            p_current_order = p_previous_order->next_order;
        } else {
            order_on_floor[p_current_order->floor] = true;
            p_previous_order = p_current_order;
            p_current_order = p_current_order->next_order;
        }
    }
}

Order* priority_queue_order_create(const int floor, const HardwareOrder direction) {
    Order* p_new_order = p_new_order = (Order*)malloc(sizeof(Order));

    p_new_order->floor = floor;
    p_new_order->direction = direction;
    p_new_order->next_order = NULL;

    return p_new_order;
}

Order* priority_queue_add_order(Order* p_new_order, Order* p_priority_queue, const int current_floor, const bool is_at_a_floor) {
    if (!p_new_order) {
        return p_priority_queue;
    }

    Order* p_updated_priority_queue = NULL;

    // Algorithm for adding order to queue
    if (priority_queue_is_empty(p_priority_queue)) {
        p_updated_priority_queue = p_new_order;
    } else {
        // If the new order is on the way to the destination, put it at the top of the queue
        const bool new_order_is_on_way = (current_floor < p_new_order->floor && p_new_order->floor < p_priority_queue->floor && p_new_order->direction != HARDWARE_ORDER_DOWN) ||
                                         (current_floor > p_new_order->floor && p_new_order->floor > p_priority_queue->floor && p_new_order->direction != HARDWARE_ORDER_UP) ||
                                         (is_at_a_floor && (p_new_order->floor == current_floor));

        if (new_order_is_on_way) {
            p_new_order->next_order = p_priority_queue;
            p_updated_priority_queue = p_new_order;
        }
        // If the new order is *not* on the way to the destination, put it at the bottom of the queue
        else {
            p_new_order->next_order = NULL;
            p_updated_priority_queue = p_priority_queue;
            priority_queue_get_last_order(p_updated_priority_queue)->next_order = p_new_order;
        }

        priority_queue_remove_duplicate_orders(p_updated_priority_queue);
    }

    return p_updated_priority_queue;
}

Order* priority_queue_pop(Order* p_first_order_of_queue, const int current_floor) {
    if (!p_first_order_of_queue) {
        return NULL;
    }

    if (!p_first_order_of_queue->next_order) {
        free(p_first_order_of_queue);
        return NULL;
    }

    Order* p_new_first_order_of_queue = p_first_order_of_queue->next_order;
    free(p_first_order_of_queue);

    // Check if orders on the bottom of the queue are compatible with the new goal by adding them back to the queue
    Order* p_remaining_orders = p_new_first_order_of_queue->next_order;
    Order* p_remaining_orders_copy = p_remaining_orders;
    p_new_first_order_of_queue->next_order = NULL;

    while (p_remaining_orders) {
        Order* p_temp_order = priority_queue_order_create(p_remaining_orders->floor, p_remaining_orders->direction);
        p_new_first_order_of_queue = priority_queue_add_order(p_temp_order, p_new_first_order_of_queue, current_floor, false);
        p_remaining_orders = p_remaining_orders->next_order;
    }

    priority_queue_clear(p_remaining_orders_copy);

    return p_new_first_order_of_queue;
}

Order* priority_queue_clear(Order* p_first_order_of_queue) {
    Order* p_traverse_order = p_first_order_of_queue;
    while (p_traverse_order) {
        Order* p_temp_order = p_traverse_order;
        p_traverse_order = p_traverse_order->next_order;
        free(p_temp_order);
    }

    return NULL;
}

bool priority_queue_is_empty(const Order* p_first_order_of_queue) { return !p_first_order_of_queue; }

void priority_queue_print(Order* p_first_order_of_queue) {
    Order* p_temp_order = p_first_order_of_queue;
    int n = 1;
    while (p_temp_order) {
        printf("%i", n);
        printf(". order\n");
        printf("Pointer address: %p\n", p_temp_order);
        printf("Next pointer address: %p\n", p_temp_order->next_order);
        printf("\t Floor: %i", p_temp_order->floor);
        printf("\n");
        printf("\t Direction: %i", (int)p_temp_order->direction);
        printf("\n");
        n++;

        p_temp_order = p_temp_order->next_order;
    }
    printf("End of queue\n");
    return;
}
