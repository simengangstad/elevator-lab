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
 * @brief Checks if @p p_new_order is on the way to @p p_current_target based on the @p current_position of the elevator.
 * 
 * @param p_new_order The order to check. 
 * @param p_current_target The current target order to check against. 
 * @param current_position The current position of the elevator. 
 * 
 * @return true if the @p p_new_order is on the way.
 */
static bool priority_queue_order_is_on_way(const Order* p_new_order, const Order* p_current_target, const Position current_position) {
    bool new_order_is_on_the_way = false;

    // In the case where the elevator is going up
    const bool new_order_is_below_target_and_has_correct_direction = (p_new_order->floor < p_current_target->floor &&
                                                                      p_new_order->direction != HARDWARE_ORDER_DOWN);
    if (new_order_is_below_target_and_has_correct_direction) {
        new_order_is_on_the_way = (current_position.floor < p_new_order->floor) ||
                                  (current_position.floor == p_new_order->floor && current_position.offset != OFFSET_ABOVE);
    }

    // In the case where the elevator is going down
    bool new_order_is_above_target_and_has_correct_direction = (p_new_order->floor > p_current_target->floor &&
                                                                p_new_order->direction != HARDWARE_ORDER_UP);
    if (new_order_is_above_target_and_has_correct_direction) {
        new_order_is_on_the_way = (p_new_order->floor < current_position.floor) ||
                                  (current_position.floor == p_new_order->floor && current_position.offset != OFFSET_BELOW);
    }

    return new_order_is_on_the_way;
}

Order* priority_queue_reorder_based_on_position(Order* p_old_priority_queue, const Position current_position) {
    if (!p_old_priority_queue) {
        return NULL;
    }

    Order* p_updated_priority_queue = priority_queue_order_create(p_old_priority_queue->floor, p_old_priority_queue->direction);
    Order* p_iterator = p_old_priority_queue->next_order;

    while (p_iterator) {
        p_updated_priority_queue = priority_queue_add_order(priority_queue_order_create(p_iterator->floor, p_iterator->direction),
                                                            p_updated_priority_queue,
                                                            current_position);
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
    p_new_order->is_oldest_order = false;

    return p_new_order;
}

Order* priority_queue_add_order(Order* p_new_order, Order* p_priority_queue, const Position current_position) {
    if (!p_new_order) {
        return p_priority_queue;
    }

    if (priority_queue_is_empty(p_priority_queue)) {
        p_new_order->is_oldest_order = true;
        return p_new_order;
    } else {
        Order* p_iterator = p_priority_queue;
        Order* p_previous_iterator = NULL;
        bool new_order_got_added = false;

        // Traverse to the oldest order and check if the new order is on way to that order
        do {
            if (priority_queue_order_is_on_way(p_new_order, p_iterator, current_position)) {
                if (!p_previous_iterator) {
                    p_new_order->next_order = p_priority_queue;
                    p_priority_queue = p_new_order;
                } else {
                    p_previous_iterator->next_order = p_new_order;
                    p_new_order->next_order = p_iterator;
                }
                new_order_got_added = true;
                break;
            }

            p_previous_iterator = p_iterator;
            p_iterator = p_iterator->next_order;

        } while (p_iterator && !(p_previous_iterator->is_oldest_order));

        // If the new order wasn't on the way to the oldest order, we add it to the bottom of the queue
        if (!new_order_got_added) {
            priority_queue_get_last_order(p_priority_queue)->next_order = p_new_order;
        }

        return priority_queue_remove_duplicate_orders(p_priority_queue);
    }
}

Order* priority_queue_pop(Order* p_priority_queue) {
    if (!p_priority_queue) {
        return NULL;
    }

    if (!p_priority_queue->next_order) {
        free(p_priority_queue);
        return NULL;
    }

    Order* p_updated_priority_queue = p_priority_queue->next_order;

    if (p_priority_queue->is_oldest_order) {
        p_updated_priority_queue->is_oldest_order = true;
    }

    free(p_priority_queue);
    return p_updated_priority_queue;

    return priority_queue_reorder(p_updated_priority_queue, current_position);
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
