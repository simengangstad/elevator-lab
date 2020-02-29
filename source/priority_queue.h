/**
 * @file
 * @brief Module for the priority queue, a linked list based on orders of #Order.
 */

#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "hardware.h"

/**
 * @brief Structure to represent an order in the queue.
 */
typedef struct Order {
    struct Order* next_order;
    int floor;
    HardwareOrder direction;
} Order;

/**
 * @brief Creates a order object representing an order by allocating space on the heap.
 *
 * @param[in] floor Floor of the new order.
 * @param[in] direction Direction of the new order.
 * 
 * @note The next_order-parameter of the *new* order is always NULL.
 *
 * @return Pointer to the newly created order.
 */
Order* order_create(const int floor, const HardwareOrder direction);

/**
 * @brief Adds an order based on a prioritation algorithm. If the queue contains duplicate orders, the lowest priority is deleted.
 *
 * @param[in] p_new_order Pointer to the order to add to the queue.
 * @param[in] p_first_order_of_queue Pointer to the first order in the queue.
 * @param[in] current_floor Current floor.
 * @param[in] is_at_a_floor Whether the elevator is currently at a floor, not between floors.
 *
 * @return Returns a pointer to the start of the queue. Returns @p p_first_order if @p p_new_order is NULL.
 */
Order* queue_add_order(Order* p_new_order, Order* p_first_order_of_queue, const int current_floor, const bool is_at_a_floor);

/**
 * @brief Deletes the first order in the queue, returns a pointer to the second order in the queue.
 *
 * @param[in] p_first_order_of_queue Pointer to the first order in the queue.
 * @param[in] current_floor Current floor of the elevator.
 *
 * @return Pointer to the second order in the queue.
 */
Order* queue_pop(Order* p_first_order_of_queue, const int current_floor);

/**
 * @brief Removes every order in the queue.
 *
 * @param[in] p_first_order_of_queue Pointer to the first order in the queue.
 * 
 * @return The new queue, will just return a null pointer.
 */
Order* queue_clear(Order* p_first_order_of_queue);

/**
 * @brief Returns true if the queue is empty.
 *
 * @param[in] p_first_order_of_queue Pointer to the first order in the queue.
 *
 * @return True if the queue is empty.
 */
bool queue_is_empty(const Order* p_first_order_of_queue);

/**
 * @brief Prints the queue to terminal, including the order floors and directions.
 *
 * @param[in] p_first_order_of_queue Pointer to first order in queue.
 */
void queue_print(Order* p_first_order_of_queue);

#endif
