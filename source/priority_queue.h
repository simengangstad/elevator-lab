/**
 * @file
 * @brief Module for the priority queue, a linked list based on orders of type #Order.
 */

#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include <stdbool.h>

#include "hardware.h"
#include "position.h"

/**
 * @brief Specifies the number of floors we're taking account for in the priority queue.
 */
#define PRIORITY_QUEUE_NUMBER_OF_FLOORS HARDWARE_NUMBER_OF_FLOORS

/**
 * @brief Structure to represent an order in the queue, a node in a linked list.
 */
typedef struct Order {
    /**
     * @brief The next order in the linked list. 
     */
    struct Order* next_order;

    /**
     * @brief The floor for this order.
     */
    int floor;

    /**
     * @brief The direction for this order.
     */
    HardwareOrder direction;

    /**
     * @brief If the order is the oldest one that got added to the priority queue, the order needs to
     *        keep track of this as the priority queue gets reordered with orders which are "on way" to
     *        the oldest order. 
     */
    bool is_oldest_order;

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
Order* priority_queue_order_create(const int floor, const HardwareOrder direction);

/**
 * @brief Adds an order based on a prioritation algorithm. If the queue contains duplicate orders, the lowest priority is deleted.
 *
 * @param[in] p_new_order Pointer to the order to add to the queue.
 * @param[in] p_priority_queue Pointer existing priority queu.
 * @param[in] current_position Current position of the elevator. Used to check where the new order should be placed.
 *
 * @return Returns a pointer to the start of the queue. Returns @p p_first_order if @p p_new_order is NULL.
 */
Order* priority_queue_add_order(Order* p_new_order, Order* p_priority_queue, const Position current_position);

/**
 * @brief Deletes the first order in the queue, returns a pointer to the second order in the queue.
 *
 * @param[in] p_priority_queue Pointer to the priority queue.
 *
 * @return Pointer to the second order in the queue.
 */
Order* priority_queue_pop(Order* p_priority_queue);

/**
 * @brief Checks if orders on the bottom of the queue are compatible with the top order by adding them into a new queue
 *        and thus running the sorting algorithm from #priority_queue_add_order on them.
 * 
 * @param [in] p_old_priority_queue The priority queue to reorder.
 * @param [in] current_position The current position.
 * 
 * @return The reordered priority queue.
 */
Order* priority_queue_reorder_based_on_position(Order* p_old_priority_queue, const Position current_position);

/**
 * @brief Removes every order in the queue.
 *
 * @param[in] p_priority_queue Pointer to the priority queue.
 * 
 * @return The new queue, will just return a null pointer.
 */
Order* priority_queue_clear(Order* p_priority_queue);

/**
 * @brief Returns true if the queue is empty.
 *
 * @param[in] p_priority_queue Pointer to the priority queue.
 *
 * @return True if the queue is empty.
 */
bool priority_queue_is_empty(const Order* p_priority_queue);

/**
 * @brief Prints the queue to terminal, including the order floors and directions.
 *
 * @param[in] p_priority_queue Pointer to priority queue.
 */
void priority_queue_print(Order* p_priority_queue);

#endif
