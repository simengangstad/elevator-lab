/**
 * @file
 * @brief Implementation of the priority queue.
 */

#include "priority_queue.h"

#include <stdio.h>
#include <stdlib.h>

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
 * 
 * @return The updated priority queue.
 */
static Order* priority_queue_remove_duplicate_orders(Order* p_priority_queue) {
    bool order_on_floor[PRIORITY_QUEUE_NUMBER_OF_FLOORS];
    for (unsigned int floor = 0; floor < sizeof(order_on_floor) / sizeof(order_on_floor[0]); floor++) {
        order_on_floor[floor] = false;
    }

    Order* p_iterator = p_priority_queue;
    Order* p_previous_order = NULL;

    while (p_iterator) {
        if (order_on_floor[p_iterator->floor]) {
            p_previous_order->next_order = p_iterator->next_order;
            free(p_iterator);
            p_iterator = p_previous_order->next_order;
        } else {
            order_on_floor[p_iterator->floor] = true;
            p_previous_order = p_iterator;
            p_iterator = p_iterator->next_order;
        }
    }

    return p_priority_queue;
}

/**
 * @brief Checks if orders on the bottom of the queue are compatible with the top order by adding them into a new queue
 *        and thus running the sorting algorithm from #priority_queue_add_order on them.
 * 
 * @param [in] p_old_priority_queue The priority queue to reorder.
 * @param [in] current_floor The current floor.
 * 
 * @return The reordered priority queue.
 */
static Order* priority_queue_reorder(Order* p_old_priority_queue, const int current_floor) {
    Order* p_updated_priority_queue = priority_queue_order_create(p_old_priority_queue->floor, p_old_priority_queue->direction);
    Order* p_iterator = p_old_priority_queue->next_order;

    while (p_iterator) {
        p_updated_priority_queue = priority_queue_add_order(priority_queue_order_create(p_iterator->floor, p_iterator->direction),
                                                            p_updated_priority_queue,
                                                            current_floor,
                                                            true);
        p_iterator = p_iterator->next_order;
    }

    priority_queue_clear(p_old_priority_queue);

    return p_updated_priority_queue;
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

    if (priority_queue_is_empty(p_priority_queue)) {
        return p_new_order;
    } else {
        Order* p_updated_priority_queue = NULL;

        const bool new_order_is_on_way = (current_floor < p_new_order->floor && p_new_order->floor < p_priority_queue->floor && p_new_order->direction != HARDWARE_ORDER_DOWN) ||
                                         (current_floor > p_new_order->floor && p_new_order->floor > p_priority_queue->floor && p_new_order->direction != HARDWARE_ORDER_UP) ||
                                         (is_at_a_floor && (p_new_order->floor == current_floor));

        // If the new order is on the way to the destination, put it at the top of the queue
        if (new_order_is_on_way) {
            p_new_order->next_order = p_priority_queue;
            p_updated_priority_queue = p_new_order;
        }
        // If the new order is *not* on the way to the destination, put it at the bottom of the queue
        else {
            p_updated_priority_queue = p_priority_queue;
            priority_queue_get_last_order(p_updated_priority_queue)->next_order = p_new_order;
        }

        return priority_queue_remove_duplicate_orders(p_updated_priority_queue);
    }
}

Order* priority_queue_pop(Order* p_priority_queue, const int current_floor) {
    if (!p_priority_queue) {
        return NULL;
    }

    if (!p_priority_queue->next_order) {
        free(p_priority_queue);
        return NULL;
    }

    Order* p_updated_priority_queue = p_priority_queue->next_order;
    free(p_priority_queue);

    return priority_queue_reorder(p_updated_priority_queue, current_floor);
}

Order* priority_queue_clear(Order* p_priority_queue) {
    Order* p_iterator = p_priority_queue;
    while (p_iterator) {
        Order* p_temp_order = p_iterator;
        p_iterator = p_iterator->next_order;
        free(p_temp_order);
    }

    return NULL;
}

bool priority_queue_is_empty(const Order* p_priority_queue) { return !p_priority_queue; }

void priority_queue_print(Order* p_priority_queue) {
    Order* p_iterator = p_priority_queue;
    int n = 1;
    while (p_iterator) {
        printf("%i", n);
        printf(". order\n");
        printf("Pointer address: %p\n", p_iterator);
        printf("Next pointer address: %p\n", p_iterator->next_order);
        printf("\t Floor: %i", p_iterator->floor);
        printf("\n");
        printf("\t Direction: %i", (int)p_iterator->direction);
        printf("\n");
        n++;

        p_iterator = p_iterator->next_order;
    }
    printf("End of queue\n");
    return;
}
