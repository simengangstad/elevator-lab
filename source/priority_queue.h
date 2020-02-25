/**
 * @file
 * @brief Library containing PriorityQueue struct and dependencies, including Node struct.
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
typedef struct Node {
    struct Node* next_node;
    int floor;
    HardwareOrder direction;
} Node;

/**
 * @brief Creates a node object representing an order by allocating space on the heap.
 *
 * @param[in] floor Floor of the new order.
 * @param[in] direction Direction of the new order.
 * 
 * @note The next_node-parameter of the *new* node is always NULL.
 *
 * @return Pointer to the newly created node.
 */
Node* node_create(const int floor, const HardwareOrder direction);

/**
 * @brief Adds an order based on a prioritation algorithm. If the queue contains duplicate orders, the lowest priority is deleted.
 *
 * @param[in] p_new_node Pointer to the order to add to the queue.
 * @param[in] p_first_node Pointer to the first order in the queue.
 * @param[in] current_floor Current floor.
 *
 * @return Returns a pointer to the start of the queue. Returns @p p_first_node if @p p_new_node is NULL.
 */
Node* queue_add_node(Node* p_new_node, const Node* p_first_node, const int current_floor);

/**
 * @brief Deletes the first order in the queue, returns a pointer to the second order in the queue.
 *
 * @param[in] p_first_node Pointer to the first order in the queue.
 * @param[in] current_floor Current floor of the elevator.
 *
 * @return Pointer to the second order in the queue.
 */
Node* queue_pop(Node* p_first_node, const int current_floor);

/**
 * @brief Removes every order in the queue.
 *
 * @param[in] p_first_node Pointer to the first order in the queue.
 * 
 * @return The new queue, will just return a null pointer.
 */
Node* queue_clear(Node* p_first_node);

/**
 * @brief Returns true if the queue is empty.
 *
 * @param[in] p_first_node Pointer to the first order in the queue.
 *
 * @return True if the queue is empty.
 */
bool queue_is_empty(const Node* p_first_node);

/**
 * @brief Prints the queue to terminal, including the order floors and directions.
 *
 * @param[in] p_first_node Pointer to first node in queue.
 */
void queue_print(Node* p_first_node);

#endif
